#include <glm/vec3.hpp>
#include <entt/entt.hpp>
#include "ECS/Systems/CollisionSystem.h"
#include "SimulationConfig.h"
#include "ECS/Components/PhysComponents.h"
#include "ECS/Components/CharComponents.h"
#include "ECS/Components/Character.h"
#include "ECS/Entity/Hierarchy.h"
#include "ECS/SystemUpdateContexts.h"
#include "Collision/CollisionEvent.h"
#include "Collision/Broad/SAP.h"
#include "Collision/Narrow/GJK/GJK.h"
#include "Event/EventDispatcher.h"
#include "GameObject/Character/CharacterMovement.h"
#include "Utils/Logger.h"
#include "Utils/RegistryEntry.h"
REGISTER_SYSTEM(tomato::SystemPhase::Collision, CollisionSystem)

namespace tomato {
    CollisionSystem::CollisionSystem()
    : broadPhase_(std::make_unique<SAP>())
    , narrowPhase_(std::make_unique<GJK>())
    {
        EventDispatcher::GetInstance().Connect<PenetrationEvent, &CollisionSystem::OnPenetration>();

        EventDispatcher::GetInstance().Connect<ChangeMovementModeEvent, &CharacterMovement::ChangeMovementMode>();
    }

    CollisionSystem::~CollisionSystem() = default;

    void CollisionSystem::Update(SimContext& simCtx)
    {
        RunBroadPhase(simCtx);
        RunNarrowPhase(simCtx);

        EventDispatcher::GetInstance().Update<PenetrationEvent>();
        ResolveCollision(simCtx.state->GetRegistry());

        EventDispatcher::GetInstance().Update<ChangeMovementModeEvent>();
    }

    void CollisionSystem::UpdateAABB(entt::registry& reg)
    {
        auto view = reg.view<ColliderComponent, TransformComponent>();
        for (auto [e, col, trf] : view.each())
        {
            if (!col.aabbDirty)
                continue;

            glm::vec3 wPos = trf.GetWorldPosition();
            // Sweep AABB
            if (const auto velPtr = reg.try_get<VelocityComponent>(GetRootEntity(reg, e)))
                wPos += velPtr->velocity * FIXED_DELTA_TIME;

            auto halfExtents = trf.GetWorldScale() * 0.5f;
            if (col.type == ColliderType::Sphere)
            {
                const glm::vec3 radius{halfExtents.x};

                col.max = wPos + radius;
                col.min = wPos - radius;
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

                col.max = wPos + aabbHalfExtents;
                col.min = wPos - aabbHalfExtents;
            }
        }
    }

    void CollisionSystem::SolveCollision(entt::registry& reg, entt::entity e1, entt::entity e2, const CollisionInfo& info)
    {
        // TMT_INFO << "=========== Solve collision " << (int)e1 << " " << (int)e2;
        entt::entity root1 = GetRootEntity(reg, e1);
        entt::entity root2 = GetRootEntity(reg, e2);

        auto& trfRoot1 = reg.get<TransformComponent>(root1);
        auto& trfRoot2 = reg.get<TransformComponent>(root2);

        // TMT_INFO << " normal: " << info.normal.x << " " << info.normal.y << " " << info.normal.z;
        if (auto* vel = reg.try_get<VelocityComponent>(root1))
        {
            // TMT_INFO << "========== " << (int)root1 << "의 " << (int)e1 << " collider ==========";
            // TMT_INFO << " 속도: " << vel->velocity.x << " " << vel->velocity.y << " " << vel->velocity.z;
            // auto pos = trfRoot1.GetLocalPosition();
            // TMT_INFO << " 위치: " << pos.x << " " << pos.y << " " << pos.z;

            glm::vec3 remainingMove = (1 - info.depth * info.weight) * vel->velocity;

            trfRoot1.AddPosition((vel->velocity * FIXED_DELTA_TIME * info.depth - info.normal * COLLISION_SKIN) * info.weight);
            vel->velocity = remainingMove - glm::dot(remainingMove, info.normal) * info.normal;

            constexpr float epsilon = 0.001f;
            if (-epsilon < vel->velocity.x && vel->velocity.x < epsilon)
                vel->velocity.x = 0.f;
            if (-epsilon < vel->velocity.y && vel->velocity.y < epsilon)
                vel->velocity.y = 0.f;
            if (-epsilon < vel->velocity.z && vel->velocity.z < epsilon)
                vel->velocity.z = 0.f;

            // TMT_INFO << " 속도: " << vel->velocity.x << " " << vel->velocity.y << " " << vel->velocity.z;
            // pos = trfRoot1.GetLocalPosition();
            // TMT_INFO << " 위치: " << pos.x << " " << pos.y << " " << pos.z;
        }

        if (auto* vel = reg.try_get<VelocityComponent>(root2))
        {
            // TMT_INFO << "========== " << (int)root2 << "의 " << (int)e2 << " collider ==========";
            // TMT_INFO << " 속도: " << vel->velocity.x << " " << vel->velocity.y << " " << vel->velocity.z;
            // auto pos = trfRoot2.GetLocalPosition();
            // TMT_INFO << " 위치: " << pos.x << " " << pos.y << " " << pos.z;

            float weight = 1 - info.weight;
            glm::vec3 remainingMove = (1 - info.depth * weight) * vel->velocity;

            trfRoot2.AddPosition((vel->velocity * FIXED_DELTA_TIME * info.depth + info.normal * COLLISION_SKIN) * weight);
            // trfRoot2.AddPosition(vel->velocity * FIXED_DELTA_TIME * info.depth + vel->velocity * COLLISION_SKIN);
            vel->velocity = remainingMove + glm::dot(remainingMove, -info.normal) * info.normal;

            constexpr float epsilon = 0.001f;
            if (-epsilon < vel->velocity.x && vel->velocity.x < epsilon)
                vel->velocity.x = 0.f;
            if (-epsilon < vel->velocity.y && vel->velocity.y < epsilon)
                vel->velocity.y = 0.f;
            if (-epsilon < vel->velocity.z && vel->velocity.z < epsilon)
                vel->velocity.z = 0.f;

            // TMT_INFO << " 속도: " << vel->velocity.x << " " << vel->velocity.y << " " << vel->velocity.z;
            // pos = trfRoot2.GetLocalPosition();
            // TMT_INFO << " 위치: " << pos.x << " " << pos.y << " " << pos.z;
        }
    }

    void CollisionSystem::OnPenetration(const PenetrationEvent& e)
    {
        // TMT_INFO << "=========== Solve penetration " << (int)e.e1 << " " << (int)e.e2;
        entt::entity root1 = GetRootEntity(*e.reg, e.e1);
        entt::entity root2 = GetRootEntity(*e.reg, e.e2);

        auto& trfRoot1 = e.reg->get<TransformComponent>(root1);
        auto& trfRoot2 = e.reg->get<TransformComponent>(root2);

        constexpr float CORRECTION_SPEED = 3.5f;

        // auto bef = trfRoot1.GetLocalPosition();
        trfRoot1.AddPosition(-e.info.normal * e.info.depth * e.info.weight * FIXED_DELTA_TIME * CORRECTION_SPEED);
        // auto aft = trfRoot1.GetLocalPosition();
        // TMT_INFO << (int)root1 << " bef: " << bef.x << " " << bef.y << " " << bef.z;
        // TMT_INFO << (int)root1 << " aft: " << aft.x << " " << aft.y << " " << aft.z;

        // bef = trfRoot2.GetLocalPosition();
        trfRoot2.AddPosition(e.info.normal * e.info.depth * (1 - e.info.weight) * FIXED_DELTA_TIME * CORRECTION_SPEED);
        // aft = trfRoot2.GetLocalPosition();
        // TMT_INFO << (int)root2 << " bef: " << bef.x << " " << bef.y << " " << bef.z;
        // TMT_INFO << (int)root2 << " aft: " << aft.x << " " << aft.y << " " << aft.z;
    }

    void CollisionSystem::RunBroadPhase(SimContext& simCtx)
    {
        auto& registry = simCtx.state->GetRegistry();

        UpdateAABB(registry);

        candidates_.clear();
        broadPhase_->FindCollisionCandidates(registry, candidates_);
    }

    void CollisionSystem::RunNarrowPhase(SimContext& simCtx)
    {
        events_.clear();

        auto& registry = simCtx.state->GetRegistry();
        auto& collisionPairs = registry.ctx().get<CollisionContext>().collisionPairs;

        auto& eventDispatcher = EventDispatcher::GetInstance();

        for (const auto& candidate : candidates_)
        {
            auto& col1 = registry.get<ColliderComponent>(candidate.a);
            auto& col2 = registry.get<ColliderComponent>(candidate.b);

            if (auto result = narrowPhase_->EvaluateCollision(registry, candidate.a, candidate.b)) {
                // Collision detected
                if (!collisionPairs.contains(candidate))
                {
                    // Enter
                    if (col1.isTrigger || col2.isTrigger)
                    {
                        eventDispatcher.Enqueue(TriggerEnterEvent{candidate.a, candidate.b, &registry});

                        if (registry.all_of<CharacterTag>(GetRootEntity(registry, candidate.a)))
                            eventDispatcher.Enqueue(ChangeMovementModeEvent{candidate.a, &registry, Walking});
                        if (registry.all_of<CharacterTag>(GetRootEntity(registry, candidate.b)))
                            eventDispatcher.Enqueue(ChangeMovementModeEvent{candidate.b, &registry, Walking});
                    }
                    else
                    {
                        events_.emplace_back(candidate.a, candidate.b, result.value());
                        eventDispatcher.Enqueue(CollisionEnterEvent{candidate.a, candidate.b, &registry, result.value()});
                    }
                }
                else
                {
                    // Stay
                    if (col1.isTrigger || col2.isTrigger)
                        eventDispatcher.Enqueue(TriggerStayEvent{candidate.a, candidate.b, &registry});
                    else
                    {
                        events_.emplace_back(candidate.a, candidate.b, result.value());
                        eventDispatcher.Enqueue(CollisionStayEvent{candidate.a, candidate.b, &registry, result.value()});
                    }
                }

                collisionPairs[candidate] = true;
            }
        }

        for (auto it = collisionPairs.begin(); it != collisionPairs.end(); )
        {
            if (!it->second)
            {
                // Exit
                auto& col1 = registry.get<ColliderComponent>(it->first.a);
                auto& col2 = registry.get<ColliderComponent>(it->first.b);

                if (col1.isTrigger || col2.isTrigger)
                {
                    EventDispatcher::GetInstance().Enqueue(TriggerExitEvent{it->first.a, it->first.b, &registry});

                    if (registry.all_of<CharacterTag>(GetRootEntity(registry, it->first.a)))
                        eventDispatcher.Enqueue(ChangeMovementModeEvent{it->first.a, &registry, Falling});
                    if (registry.all_of<CharacterTag>(GetRootEntity(registry, it->first.b)))
                        eventDispatcher.Enqueue(ChangeMovementModeEvent{it->first.b, &registry, Falling});
                }
                else
                    EventDispatcher::GetInstance().Enqueue(CollisionExitEvent{it->first.a, it->first.b, &registry});

                it = collisionPairs.erase(it);
            }
            else
            {
                it->second = false;
                ++it;
            }
        }
    }

    void CollisionSystem::ResolveCollision(entt::registry& reg)
    {
        for (auto& event : events_)
        {
            SolveCollision(reg, event.e1, event.e2, event.info);
        }
    }
}
