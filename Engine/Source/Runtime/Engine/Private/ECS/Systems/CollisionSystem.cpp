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
    , narrowPhase_(std::make_unique<GJK>())
    {
        auto& eventDispatcher = EventDispatcher::GetInstance();
        eventDispatcher.Connect<CollisionEnterEvent>();
        eventDispatcher.Connect<CollisionStayEvent>();
        eventDispatcher.Connect<CollisionExitEvent>();

        eventDispatcher.Connect<TriggerEnterEvent>();
        eventDispatcher.Connect<TriggerStayEvent>();
        eventDispatcher.Connect<TriggerExitEvent>();
    }

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

        auto& registry = simCtx.state->GetRegistry();

        UpdateAABB(registry);
        broadPhase_->FindContactPairCandidates(registry, candidates_);
    }

    void CollisionSystem::RunNarrowPhase(SimContext& simCtx)
    {
        contacts_.clear();

        auto& registry = simCtx.state->GetRegistry();
        auto& contactPairs = registry.ctx().get<CollisionContext>().pairs;
        auto& eventDispatcher = EventDispatcher::GetInstance();

        // Check contact pair candidates
        for (const auto& candidate : candidates_)
        {
            if (!registry.valid(candidate.a) || !registry.valid(candidate.b))
                continue;

            if (auto result = narrowPhase_->EvaluateContactPair(registry, candidate))
            {
                // Collision detected
                if (!contactPairs.contains(candidate))
                {
                    contactPairs[candidate].normal = result->normal;

                    // Enter
                    if (result->trigger)
                    {
                        std::cout << "      trg ENTER " << candidate << "\n";
                        eventDispatcher.Enqueue(TriggerEnterEvent{candidate.a, candidate.b, &registry});
                    }
                    else
                    {
                        std::cout << "      col ENTER " << candidate << "\n";
                        eventDispatcher.Enqueue(CollisionEnterEvent{candidate.a, candidate.b, &registry, result.value()});
                        contacts_.push_back(ContactEvent{candidate.a, candidate.b, &registry, result.value()});
                    }
                }
                else
                {
                    if (result->distance < COLLISION_SKIN + 1e-4f)
                        result->normal = contactPairs[candidate].normal;
                    else
                        contactPairs[candidate].normal = result->normal;

                    // Stay
                    if (result->trigger)
                    {
                        // std::cout << "      trg STAY " << candidate << "\n";
                        eventDispatcher.Enqueue(TriggerStayEvent{candidate.a, candidate.b, &registry});
                    }
                    else
                    {
                        // std::cout << "      col STAY " << candidate << "\n";
                        eventDispatcher.Enqueue(CollisionStayEvent{candidate.a, candidate.b, &registry, result.value()});
                        contacts_.emplace_back(ContactEvent{candidate.a, candidate.b, &registry, result.value()});
                    }
                }

                contactPairs[candidate].exitCnt = EXIT_CNT;
            }
        }

        // Check exit of contact pairs
        for (auto it = contactPairs.begin(); it != contactPairs.end(); )
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
                        std::cout << "      trg EXIT " << it->first << "\n";
                        EventDispatcher::GetInstance().Enqueue(TriggerExitEvent{ it->first.a, it->first.b, &registry });
                    }
                    else
                    {
                        std::cout << "      col EXIT " << it->first << "\n";
                        EventDispatcher::GetInstance().Enqueue(CollisionExitEvent{ it->first.a, it->first.b, &registry });
                    }
                }

                it = contactPairs.erase(it);
            }
            else
            {
                --it->second.exitCnt;
                ++it;
            }
        }
    }

    void CollisionSystem::UpdateAABB(entt::registry& reg)
    {
        auto view = reg.view<ColliderComponent, TransformComponent>();
        for (auto [e, col, trf] : view.each())
        {
            if (!col.aabbDirty)
                continue;
            col.aabbDirty = false;

            glm::vec3 wPos = trf.GetWorldPosition();
            // Sweep AABB
            if (const auto velPtr = reg.try_get<VelocityComponent>(GetRootEntity(reg, e)))
                wPos += velPtr->velocity * FIXED_DELTA_TIME;

            auto halfExtents = trf.GetWorldScale() * 0.5f;
            if (col.type == ColliderType::Sphere)
            {
                const glm::vec3 radius{halfExtents.x};

                col.max = wPos + radius + HALF_COLLISION_SKIN + 1e-6f;
                col.min = wPos - radius - HALF_COLLISION_SKIN - 1e-6f;
            }
            else
            {
                auto R = glm::toMat4(trf.GetWorldQuaternion());

                glm::vec3 aabbHalfExtents
                {
                    glm::abs(R[0][0]) * halfExtents.x + glm::abs(R[1][0]) * halfExtents.y + glm::abs(R[2][0]) * halfExtents.z,
                    glm::abs(R[0][1]) * halfExtents.x + glm::abs(R[1][1]) * halfExtents.y + glm::abs(R[2][1]) * halfExtents.z,
                    glm::abs(R[0][2]) * halfExtents.x + glm::abs(R[1][2]) * halfExtents.y + glm::abs(R[2][2]) * halfExtents.z
                };

                col.max = wPos + aabbHalfExtents + HALF_COLLISION_SKIN + 1e-6f;
                col.min = wPos - aabbHalfExtents - HALF_COLLISION_SKIN - 1e-6f;
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
//        std::cout << " ===== SOLVE COLLISION " << (int)event.e1 << " " << (int)event.e2 << "\n";
//        std::cout << "       normal: " << event.data.normal.x << " " << event.data.normal.y << " " << event.data.normal.z << "\n";

        auto& reg = *(event.reg);
        entt::entity root1 = GetRootEntity(reg, event.e1);
        entt::entity root2 = GetRootEntity(reg, event.e2);

        glm::vec3 v1{0.f};
        glm::vec3 v2{0.f};

        auto vel1 = reg.try_get<VelocityComponent>(root1);
        if (vel1)
            v1 = vel1->velocity;

        auto vel2 = reg.try_get<VelocityComponent>(root2);
        if (vel2)
            v2 = vel2->velocity;

        float lenV1 = glm::length(v1);
        float lenV2 = glm::length(v2);
        float sumV = lenV1 + lenV2;

        float weight1 = 0.5f;
        if (sumV >= 1e-6f)
            weight1 = lenV1 / sumV;
//        std::cout << "       weight1: " << weight1 << "\n";
        float weight2 = 1 - weight1;

        auto& trfRoot1 = reg.get<TransformComponent>(root1);
        auto& trfRoot2 = reg.get<TransformComponent>(root2);

        if (event.data.hitTime.has_value())
        {
            // CCD contact data

            if (vel1)
                ResolveContinuousContact(trfRoot1, *vel1, -event.data.normal, weight1, event.data.hitTime.value());

            if (vel2)
                ResolveContinuousContact(trfRoot2, *vel2, event.data.normal, weight2, event.data.hitTime.value());
        }
        else if (event.data.distance >= 0)
        {
            // COLLISION_SKIN 만큼만 떨어져 있어서 GJK Distance로부터 받은 충돌 정보

            if (vel1)
                ResolveDiscreteContact(trfRoot1, *vel1, -event.data.normal, weight1, event.data.distance);

            if (vel2)
                ResolveDiscreteContact(trfRoot2, *vel2, event.data.normal, weight2, event.data.distance);
        }
        else
        {
            // 겹침 보정
            ResolvePenetration(trfRoot1, -event.data.normal, weight1, event.data.distance);
            ResolvePenetration(trfRoot2,  event.data.normal, weight2, event.data.distance);
        }
    }

    void CollisionSystem::ResolveContinuousContact(
        TransformComponent& trf, VelocityComponent& vel,
        const glm::vec3& normal, const float weight, const float hitTime)
    {
//        std::cout << "          position 1: " << glm::to_string(trf.GetLocalPosition()) << "\n";

        trf.AddPosition((vel.velocity * FIXED_DELTA_TIME * hitTime + normal * COLLISION_SKIN) * weight);
//        std::cout << "          position C: " << glm::to_string(trf.GetLocalPosition()) << "\n";

        glm::vec3 remainingMove = (1 - hitTime * weight) * vel.velocity;
        vel.velocity = remainingMove + glm::dot(remainingMove, -normal) * normal;

        if (-EPSILON < vel.velocity.x && vel.velocity.x < EPSILON)
            vel.velocity.x = 0.f;
        if (-EPSILON < vel.velocity.y && vel.velocity.y < EPSILON)
            vel.velocity.y = 0.f;
        if (-EPSILON < vel.velocity.z && vel.velocity.z < EPSILON)
            vel.velocity.z = 0.f;
//        std::cout << "          velocity C: " << glm::to_string(vel.velocity) << "\n";
    }

    void CollisionSystem::ResolveDiscreteContact(
        TransformComponent& trf, VelocityComponent& vel,
        const glm::vec3& normal, const float weight, const float distance)
    {
//        std::cout << "          position 1: " << glm::to_string(trf.GetLocalPosition()) << "\n";

        trf.AddPosition(normal * (COLLISION_SKIN - distance) * weight);
//        std::cout << "          position D: " << glm::to_string(trf.GetLocalPosition()) << "\n";

        vel.velocity += glm::dot(vel.velocity, -normal) * normal;

        if (-EPSILON < vel.velocity.x && vel.velocity.x < EPSILON)
            vel.velocity.x = 0.f;
        if (-EPSILON < vel.velocity.y && vel.velocity.y < EPSILON)
            vel.velocity.y = 0.f;
        if (-EPSILON < vel.velocity.z && vel.velocity.z < EPSILON)
            vel.velocity.z = 0.f;
//        std::cout << "          velocity D: " << glm::to_string(vel.velocity) << "\n";
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
