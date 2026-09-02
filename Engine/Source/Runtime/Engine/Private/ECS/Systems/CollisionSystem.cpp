#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec3.hpp>
#include <glm/gtx/string_cast.hpp>
#include <entt/entt.hpp>
#include "ECS/Systems/CollisionSystem.h"
#include "ECS/Components/ComponentsPhys.h"
#include "ECS/Entity/Hierarchy.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "Collision/CollisionEvent.h"
#include "Collision/CollisionConfig.h"
#include "Collision/Broad/SAP.h"
#include "Collision/Narrow/GJK/GJK.h"
#include "Simulation/SimulationConfig.h"
#include "Event/EventDispatcher.h"
#include "Utils/Logger.h"

namespace tomato
{
    CollisionSystem::CollisionSystem()
    : broadPhase_(std::make_unique<SAP>())
    , narrowPhase_(std::make_unique<GJK>()) {}

    CollisionSystem::~CollisionSystem() = default;

    void CollisionSystem::Update(SimContext& simCtx)
    {
        RunBroadPhase(simCtx);
        RunNarrowPhase(simCtx);

        ResolveContacts();

        UpdateCollisionEvents();
    }

    void CollisionSystem::RunBroadPhase(SimContext& simCtx)
    {
        candidates_.clear();

        broadPhase_->FindContactPairCandidates(simCtx.state->GetRegistry(), candidates_);
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
            if (auto result = narrowPhase_->EvaluateContactPair(registry, candidate))
            {
                // Collision detected

                if (!contactCache.contains(candidate))
                {
                    // Enter
                    // Detect new contact pair
                    contactCache[candidate].normal = result->normal;

                    if (result->trigger)
                    {
                         std::cout << "      trg ENTER " << candidate << "\n";
                        eventDispatcher.Enqueue(TriggerEnterEvent{candidate.a, candidate.b, &registry});
                    }
                    else
                    {
                         std::cout << "      col ENTER " << candidate << "\n";
                        eventDispatcher.Enqueue(
                            CollisionEnterEvent{candidate.a, candidate.b, &registry, result.value()});
                        contacts_.push_back(ContactEvent{candidate.a, candidate.b, &registry, result.value()});
                    }
                }
                else
                {
                    // Stay
                    if (result->distance < COLLISION_SKIN + 1e-4f)
                        result->normal = contactCache[candidate].normal;
                    else
                        contactCache[candidate].normal = result->normal;

                    if (result->trigger)
                    {
                        // std::cout << "      trg STAY " << candidate << "\n";
                        eventDispatcher.Enqueue(TriggerStayEvent{candidate.a, candidate.b, &registry});
                    }
                    else
                    {
                        // std::cout << "      col STAY " << candidate << "\n";
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
                        // std::cout << "      trg EXIT " << it->first << "\n";
                        EventDispatcher::GetInstance().Enqueue(TriggerExitEvent{ it->first.a, it->first.b, &registry });
                    }
                    else
                    {
                        // std::cout << "      col EXIT " << it->first << "\n";
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
        // std::cout << " ===== SOLVE COLLISION " << (int)event.a << " " << (int)event.b << "\n";
        // std::cout << "       normal: " << glm::to_string(event.data.normal) << "\n";

        auto& reg = *(event.reg);
        entt::entity rootA = GetRootEntity(reg, event.a);
        entt::entity rootB = GetRootEntity(reg, event.b);

        glm::vec3 vA{0.f};
        glm::vec3 vB{0.f};

        auto velA = reg.try_get<VelocityComponent>(rootA);
        if (velA)
            vA = velA->velocity;

        auto velB = reg.try_get<VelocityComponent>(rootB);
        if (velB)
            vB = velB->velocity;

        float lenVA = glm::length2(vA);
        float lenVB = glm::length2(vB);
        float sumV = lenVA + lenVB;

        float weightA = 0.5f;
        if (velA && !velB)
            weightA = 1;
        else if (!velA && velB)
            weightA = 0;
        else if (sumV >= EPSILON_SQ)
            weightA = lenVA / sumV;
         // std::cout << "       weightA: " << weightA << "\n";
        float weightB = 1 - weightA;

        auto& trfRootA = reg.get<TransformComponent>(rootA);
        auto& trfRootB = reg.get<TransformComponent>(rootB);

        if (event.data.hitTime.has_value())
        {
            // CCD contact data

            if (velA)
                ResolveContinuousContact(trfRootA, *velA, -event.data.normal, weightA, event.data.hitTime.value());

            if (velB)
                ResolveContinuousContact(trfRootB, *velB, event.data.normal, weightB, event.data.hitTime.value());
        }
        else if (event.data.distance >= 0)
        {
            // COLLISION_SKIN 만큼만 떨어져 있어서 GJK Distance로부터 받은 충돌 정보

            if (velA)
                ResolveDiscreteContact(trfRootA, *velA, -event.data.normal, weightA, event.data.distance);

            if (velB)
                ResolveDiscreteContact(trfRootB, *velB, event.data.normal, weightB, event.data.distance);
        }
        else
        {
            // 겹침 보정
            ResolvePenetration(trfRootA, -event.data.normal, weightA, event.data.distance);
            ResolvePenetration(trfRootB,  event.data.normal, weightB, event.data.distance);
        }
    }

    void CollisionSystem::ResolveContinuousContact(
        TransformComponent& trf, VelocityComponent& vel,
        const glm::vec3& normal, const float weight, const float hitTime)
    {
        // std::cout << "   CC     " << glm::to_string(normal) << " " << weight << " " << hitTime << "\n";
        // std::cout << "          position 1: " << glm::to_string(trf.GetLocalPosition()) << "\n";

        // Move
        if (hitTime == 0)
            trf.AddPosition(normal * EPSILON);
        else
            trf.AddPosition((vel.velocity * FIXED_DELTA_TIME * hitTime + normal * COLLISION_SKIN) * weight);
        // std::cout << "          position C: " << glm::to_string(trf.GetLocalPosition()) << "\n";

        // Slide
        glm::vec3 remainingMove = (1 - hitTime * weight) * vel.velocity;
        vel.velocity = remainingMove + glm::dot(remainingMove, -normal) * normal;

        if (-EPSILON < vel.velocity.x && vel.velocity.x < EPSILON)
            vel.velocity.x = 0.f;
        if (-EPSILON < vel.velocity.y && vel.velocity.y < EPSILON)
            vel.velocity.y = 0.f;
        if (-EPSILON < vel.velocity.z && vel.velocity.z < EPSILON)
            vel.velocity.z = 0.f;
        // std::cout << "          velocity C: " << glm::to_string(vel.velocity) << "\n";
    }

    void CollisionSystem::ResolveDiscreteContact(
        TransformComponent& trf, VelocityComponent& vel,
        const glm::vec3& normal, const float weight, const float distance)
    {
        // std::cout << "    DC    " << glm::to_string(normal) << " " << weight << " " << distance << "\n";
        // std::cout << "          position 1: " << glm::to_string(trf.GetLocalPosition()) << "\n";

        // Move
        float moveDist = COLLISION_SKIN - distance;
        if (moveDist < EPSILON_SQ)
            moveDist = EPSILON_SQ;
        trf.AddPosition(normal * moveDist * weight);
        // std::cout << "          position D: " << glm::to_string(trf.GetLocalPosition()) << "\n";

        // Slide
        float lenV = glm::length(vel.velocity);
        if (lenV > EPSILON)
        {
            float hitTime = distance / lenV;
            glm::vec3 remainingMove = (1 - hitTime * weight) * vel.velocity;
            vel.velocity = remainingMove + glm::dot(remainingMove, -normal) * normal;

            if (-EPSILON < vel.velocity.x && vel.velocity.x < EPSILON)
                vel.velocity.x = 0.f;
            if (-EPSILON < vel.velocity.y && vel.velocity.y < EPSILON)
                vel.velocity.y = 0.f;
            if (-EPSILON < vel.velocity.z && vel.velocity.z < EPSILON)
                vel.velocity.z = 0.f;
        }
        // std::cout << "          velocity D: " << glm::to_string(vel.velocity) << "\n";
    }

    void CollisionSystem::ResolvePenetration(
        TransformComponent& trf,
        const glm::vec3& normal, const float weight, const float distance)
    {
//        std::cout << "         position 1: " << glm::to_string(trf.GetLocalPosition()) << "\n";
        trf.AddPosition(normal * -distance * weight * FIXED_DELTA_TIME * CORRECTION_SPEED);
//        std::cout << "         position P: " << glm::to_string(trf.GetLocalPosition()) << "\n";
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
