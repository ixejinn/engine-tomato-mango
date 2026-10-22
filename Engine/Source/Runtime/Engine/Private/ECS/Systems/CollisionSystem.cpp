#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec3.hpp>
#include <glm/gtx/string_cast.hpp>
#include <entt/entt.hpp>
#include "ECS/Systems/CollisionSystem.h"
#include "ECS/Components/ComponentsPhys.h"
#include "ECS/Entity/Hierarchy.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "Collision/CollisionEvent.h"
#include "Collision/CollisionConstants.h"
#include "Collision/CollisionTolerance.h"
#include "Collision/CollisionDebug.h"
#include "Collision/Detection/Broad/SAP.h"
#include "../../Collision/Detection/Narrow/GJK/GJK.h"
#include "Simulation/SimulationConfig.h"
#include "Event/EventDispatcher.h"
#include "Utils/Logger.h"
#include "Profiler/CPUProfiler.h"

namespace
{
    glm::vec3 ComputeSlideVector(const glm::vec3& v, const glm::vec3& n)
    {
        const float dot = glm::dot(v, n);
        return dot < 0 ? v - dot * n : v;
    }
}

namespace tomato
{
    CollisionSystem::~CollisionSystem() = default;

    void CollisionSystem::Update(SimContext& simCtx)
    {
        CPU_PROFILER_BLOCK_BEGIN(CollisionSystem::Update);
        RunBroadPhase(simCtx);
        RunNarrowPhase(simCtx);

        ResolveContacts();

        UpdateCollisionEvents();
        CPU_PROFILER_BLOCK_END(CollisionSystem::Update);
    }

    void CollisionSystem::RunBroadPhase(SimContext& simCtx)
    {
        candidates_.clear();
        broad_.FindContactPairCandidates(simCtx.state->GetRegistry(), candidates_);
    }

    void CollisionSystem::RunNarrowPhase(SimContext& simCtx)
    {
        contacts_.clear();

        auto& registry = simCtx.state->GetRegistry();
        auto& contactCache = registry.ctx().get<CollisionContext>().cacheMap;
        auto& eventDispatcher = EventDispatcher::GetInstance();

        // Check contact pair candidates
        for (const auto& candidate : candidates_)
        {
            if (!registry.valid(candidate.a) || !registry.valid(candidate.b))
                continue;

            // Test narrow phase
            if (auto result = narrow_.EvaluateContactPair(registry, candidate))
            {
                // Collision detected

                if (!contactCache.contains(candidate))
                {
                    // Enter
                    // Detect new contact pair
                    if (result->trigger)
                    {
                        COL_DETECT_LOG("      TRG ENTER " << candidate);

                        eventDispatcher.Enqueue(TriggerEnterEvent{candidate.a, candidate.b, &registry});
                    }
                    else
                    {
                        COL_DETECT_LOG("      COL ENTER " << candidate);

                        eventDispatcher.Enqueue(
                            CollisionEnterEvent{candidate.a, candidate.b, &registry, result.value()});
                        contacts_.push_back(ContactEvent{candidate.a, candidate.b, &registry, result.value()});
                    }
                }
                else
                {
                    // Stay
                    if (result->trigger)
                    {
                        COL_DETECT_VERBOSE_LOG("      TRG STAY  " << candidate);

                        eventDispatcher.Enqueue(TriggerStayEvent{candidate.a, candidate.b, &registry});
                    }
                    else
                    {
                        COL_DETECT_VERBOSE_LOG("      COL STAY  " << candidate);

                        eventDispatcher.Enqueue(CollisionStayEvent{
                            candidate.a, candidate.b, &registry, result.value()
                        });
                        contacts_.emplace_back(ContactEvent{candidate.a, candidate.b, &registry, result.value()});
                    }
                }

                contactCache[candidate].exitCnt = EXIT_CNT;
            }
        }

        // Check exit of contact pairs
        for (auto it = contactCache.begin(); it != contactCache.end(); )
        {
            if (it->second.exitCnt <= 0)
            {
                // Exit
                auto* col1 = registry.try_get<ColliderComponent>(it->first.a);
                auto* col2 = registry.try_get<ColliderComponent>(it->first.b);

                if (col1 && col2)
                {
                    if (col1->trigger || col2->trigger)
                    {
                        COL_DETECT_LOG("      TRG EXIT  " << it->first);

                        EventDispatcher::GetInstance().Enqueue(TriggerExitEvent{ it->first.a, it->first.b, &registry });
                    }
                    else
                    {
                        COL_DETECT_LOG("      COL EXIT  " << it->first);

                        EventDispatcher::GetInstance().Enqueue(CollisionExitEvent{ it->first.a, it->first.b, &registry });
                    }
                }

                it = contactCache.erase(it);
            }
            else
            {
                --it->second.exitCnt;
                ++it;
            }
        }
    }

    void CollisionSystem::ResolveContacts()
    {
        for (auto& contact : contacts_)
            ResolveContact(contact);
    }

    void CollisionSystem::ResolveContact(ContactEvent& event)
    {
        COL_RESOLVE_LOG("===== SOLVE COLLISION " << (int)event.a << " " << (int)event.b);

        auto& reg = *(event.reg);
        entt::entity rootA = GetRootEntity(reg, event.a);
        entt::entity rootB = GetRootEntity(reg, event.b);

        auto& trfRootA = reg.get<TransformComponent>(rootA);
        auto& trfRootB = reg.get<TransformComponent>(rootB);

        auto* velA = reg.try_get<VelocityComponent>(rootA);
        auto* velB = reg.try_get<VelocityComponent>(rootB);

        auto* bodyA = reg.try_get<PhysicsBodyComponent>(rootA);
        auto* bodyB = reg.try_get<PhysicsBodyComponent>(rootB);

        float invMassA{0};
        float invMassB{0};

        if (velA && bodyA)
            invMassA = bodyA->inverseMass;
        if (velB && bodyB)
            invMassB = bodyB->inverseMass;

        float wA = invMassA / (invMassA + invMassB);
        float wB = 1 - wA;
        COL_RESOLVE_LOG("      weightA: " << wA);

        if (event.data.hitTime.has_value())
        {
            // CCD contact data
            if (wA > 0)
                ResolveContinuousContact(trfRootA, *velA, -event.data.normal, wA, event.data.hitTime.value());

            if (wB > 0)
                ResolveContinuousContact(trfRootB, *velB, event.data.normal, wB, event.data.hitTime.value());
        }
        else
        {
            // COLLISION_SKIN 만큼만 떨어져 있거나 겹치는 경우 GJK Distance로부터 받은 충돌 정보
            if (wA > 0)
                ResolveDiscreteContact(trfRootA, *velA, -event.data.normal, wA, event.data.distance);

            if (wB > 0)
                ResolveDiscreteContact(trfRootB, *velB, event.data.normal, wB, event.data.distance);
        }
    }

    void CollisionSystem::ResolveContinuousContact(
        TransformComponent& trf, VelocityComponent& vel,
        const glm::vec3& normal, const float weight, const float hitTime)
    {
        COL_RESOLVE_LOG("===== CC " << glm::to_string(normal) << " " << weight << " " << hitTime);

        // Move
        COL_RESOLVE_LOG("      pos before: " << glm::to_string(trf.GetLocalPosition()));
        trf.AddPosition(vel.Get() * FIXED_DELTA_TIME * hitTime + normal * COLLISION_SKIN * weight);
        COL_RESOLVE_LOG("      pos after : " << glm::to_string(trf.GetLocalPosition()));

        // Slide
        COL_RESOLVE_LOG("      vel before: " << glm::to_string(vel.Get()));
        glm::vec3 remainingInput = (1 - hitTime) * vel.input;
        vel.input = ComputeSlideVector(remainingInput, normal);

        glm::vec3 remainingMove = (1 - hitTime) * vel.velocity;
        vel.velocity = ComputeSlideVector(remainingMove, normal);
        COL_RESOLVE_LOG("      vel after : " << glm::to_string(vel.Get()));
    }

    void CollisionSystem::ResolveDiscreteContact(
        TransformComponent& trf, VelocityComponent& vel,
        const glm::vec3& normal, const float weight, const float distance)
    {
        COL_RESOLVE_LOG("===== DC " << glm::to_string(normal) << " " << weight << " " << distance);

        // Move
        COL_RESOLVE_LOG("      pos before: " << glm::to_string(trf.GetLocalPosition()));
        float moveDist = COLLISION_SKIN - distance;
        if (moveDist > 0)
            trf.AddPosition(normal * moveDist * weight);
        COL_RESOLVE_LOG("      pos after : " << glm::to_string(trf.GetLocalPosition()));

        // Slide
        COL_RESOLVE_LOG("      vel before: " << glm::to_string(vel.Get()));
        float lenI = glm::length(vel.input);
        if (lenI > 0)
        {
            float hitTime = distance / lenI;
            glm::vec3 remainingInput = (1 - hitTime * weight) * vel.input;
            vel.input = ComputeSlideVector(remainingInput, normal);
        }

        float lenV = glm::length(vel.velocity);
        if (lenV > 0)
        {
            float hitTime = distance / lenV;
            glm::vec3 remainingMove = (1 - hitTime * weight) * vel.velocity;
            vel.velocity = ComputeSlideVector(remainingMove, normal);
        }
        COL_RESOLVE_LOG("      vel after : " << glm::to_string(vel.Get()));
    }

    void CollisionSystem::UpdateCollisionEvents()
    {
        auto& eventDispatcher = EventDispatcher::GetInstance();
        eventDispatcher.Update<CollisionEnterEvent>();
        eventDispatcher.Update<CollisionStayEvent>();
        eventDispatcher.Update<CollisionExitEvent>();

        eventDispatcher.Update<TriggerEnterEvent>();
        eventDispatcher.Update<TriggerStayEvent>();
        eventDispatcher.Update<TriggerExitEvent>();
    }
}
