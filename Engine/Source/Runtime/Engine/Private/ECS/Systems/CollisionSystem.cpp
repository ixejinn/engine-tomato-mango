#include <entt/entt.hpp>
#include "ECS/Systems/CollisionSystem.h"

#include "SimulationConfig.h"
#include "ECS/Components/Collision.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/SystemUpdateContexts.h"
#include "Collision/CollisionEvent.h"
#include "Collision/Broad/SAP.h"
#include "Collision/Narrow/GJK.h"
#include "ECS/Components/Rigidbody.h"
#include "ECS/Systems/KinematicMovementSystem.h"
#include "Event/EventDispatcher.h"
#include "Utils/Logger.h"
#include "Utils/RegistryEntry.h"
REGISTER_SYSTEM(tomato::SystemPhase::Collision, CollisionSystem)

namespace tomato {
    CollisionSystem::CollisionSystem()
        : broadPhase_(std::make_unique<SAP>()), narrowPhase_(std::make_unique<GJK>()) {
        EventDispatcher::GetInstance().Connect<CollisionEnterEvent, &CollisionSystem::OnCollisionEnter>();
        EventDispatcher::GetInstance().Connect<CollisionStayEvent, &CollisionSystem::OnCollisionStay>();
        EventDispatcher::GetInstance().Connect<CollisionExitEvent, &CollisionSystem::OnCollisionExit>();

        EventDispatcher::GetInstance().Connect<TriggerEnterEvent, &CollisionSystem::OnTriggerEnter>();
        EventDispatcher::GetInstance().Connect<TriggerStayEvent, &CollisionSystem::OnTriggerStay>();
        EventDispatcher::GetInstance().Connect<TriggerExitEvent, &CollisionSystem::OnTriggerExit>();
    }

    CollisionSystem::~CollisionSystem() = default;

    void CollisionSystem::Update(SimContext& simCtx) {
        candidates_.clear();

        DetectBroad(simCtx.registry);
        DetectNarrow(simCtx.registry);
    }

    void CollisionSystem::DetectBroad(entt::registry &reg) {
        auto group = reg.group<ColliderComponent>();

        for (auto [e, col] : group.each())
        {
            if (col.aabbDirty)
                // Update AABB min, max
                SetAABB(reg, e);
        }

        broadPhase_->DetectCollision(reg, candidates_);
    }

    void CollisionSystem::DetectNarrow(entt::registry& reg) {
        for (const auto& candidate : candidates_) {
            auto& col1 = reg.get<ColliderComponent>(candidate.a);
            auto& col2 = reg.get<ColliderComponent>(candidate.b);

            // auto& trf1 = reg.get<TransformComponent>(candidate.a);
            // auto& trf2 = reg.get<TransformComponent>(candidate.b);

            if (auto result = narrowPhase_->DetectCollision(reg, candidate.a, candidate.b)) {
                if (!col1.isTrigger && !col2.isTrigger) {
                    entt::entity rootA = GetRootEntity(reg, candidate.a);
                    entt::entity rootB = GetRootEntity(reg, candidate.b);

                    auto& trfRootA = reg.get<TransformComponent>(rootA);
                    auto& trfRootB = reg.get<TransformComponent>(rootB);

                    if (auto* velPtr = reg.try_get<VelocityComponent>(rootA)) {
                        glm::vec3 remainingMove = (1 - result->depth) * velPtr->velocity;

                        trfRootA.AddPosition(velPtr->velocity * FIXED_DELTA_TIME * result->depth - result->normal * COLLISION_SKIN);
                        velPtr->velocity = remainingMove - glm::dot(remainingMove, result->normal) * result->normal;
                    }

                    if (auto* velPtr = reg.try_get<VelocityComponent>(rootB)) {
                        glm::vec3 remainingMove = (1 - result->depth) * velPtr->velocity;

                        trfRootB.AddPosition(velPtr->velocity * FIXED_DELTA_TIME * result->depth + result->normal * COLLISION_SKIN);
                        velPtr->velocity = remainingMove + glm::dot(remainingMove, -result->normal) * result->normal;
                    }
                }

                // Collision detected
                if (!collisionPairs_.contains(candidate)) {
                    // Enter
                    if (col1.isTrigger || col2.isTrigger)
                        EventDispatcher::GetInstance().Enqueue(TriggerEnterEvent{candidate.a, candidate.b, &reg});
                    else
                        EventDispatcher::GetInstance().Enqueue(CollisionEnterEvent{candidate.a, candidate.b, &reg, result->normal, result->depth});
                }
                else {
                    // Stay
                    if (col1.isTrigger || col2.isTrigger)
                        EventDispatcher::GetInstance().Enqueue(TriggerStayEvent{candidate.a, candidate.b, &reg});
                    else
                        EventDispatcher::GetInstance().Enqueue(CollisionStayEvent{candidate.a, candidate.b, &reg, result->normal, result->depth});
                }

                collisionPairs_[candidate] = true;
            }
        }

        for (auto it = collisionPairs_.begin(); it != collisionPairs_.end(); ) {
            if (!it->second) {
                // Exit
                auto& col1 = reg.get<ColliderComponent>(it->first.a);
                auto& col2 = reg.get<ColliderComponent>(it->first.b);

                if (col1.isTrigger || col2.isTrigger)
                    EventDispatcher::GetInstance().Enqueue(TriggerExitEvent{it->first.a, it->first.b, &reg});
                else
                    EventDispatcher::GetInstance().Enqueue(CollisionExitEvent{it->first.a, it->first.b, &reg});

                it = collisionPairs_.erase(it);
            }
            else {
                it->second = false;
                ++it;
            }
        }
    }

    void CollisionSystem::SetAABB(entt::registry& reg, const entt::entity e) {
        auto& col = reg.get<ColliderComponent>(e);
        auto& trf = reg.get<TransformComponent>(e);

        glm::vec3 wPos = trf.GetWorldPosition();
        // Sweep AABB
        if (const auto velPtr = reg.try_get<VelocityComponent>(GetRootEntity(reg, e)))
            wPos += velPtr->velocity * FIXED_DELTA_TIME;

        auto halfExtents = trf.GetWorldScale() * 0.5f;
        if (col.type == ColliderType::Sphere) {
            const glm::vec3 radius{halfExtents.x};

            col.max = wPos + radius;
            col.min = wPos - radius;
        }
        else {
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

    void CollisionSystem::OnCollisionEnter(const CollisionEnterEvent &e) {
        TMT_INFO << "Collision Enter: " << (uint32_t)e.e1 << ", " << (uint32_t)e.e2 <<
            " (" << e.depth << " : " << e.normal.x << "," << e.normal.y << "," << e.normal.z << ")";

        auto callback = e.reg->try_get<OnCollisionComponent>(e.e1);
        if (callback && callback->enter)
            callback->enter(e, e.e1);

        callback = e.reg->try_get<OnCollisionComponent>(e.e2);
        if (callback && callback->enter)
            callback->enter(e, e.e2);
    }

    void CollisionSystem::OnCollisionStay(const CollisionStayEvent& e) {
//        TMT_INFO << "Collision Stay";

        auto callback = e.reg->try_get<OnCollisionComponent>(e.e1);
        if (callback && callback->stay)
            callback->stay(e, e.e1);

        callback = e.reg->try_get<OnCollisionComponent>(e.e2);
        if (callback && callback->stay)
            callback->stay(e, e.e2);
    }

    void CollisionSystem::OnCollisionExit(const CollisionExitEvent& e) {
        TMT_INFO << "Collision Exit : " << (uint32_t)e.e1 << ", " << (uint32_t)e.e2;

        auto callback = e.reg->try_get<OnCollisionComponent>(e.e1);
        if (callback && callback->exit)
            callback->exit(e, e.e1);

        callback = e.reg->try_get<OnCollisionComponent>(e.e2);
        if (callback && callback->exit)
            callback->exit(e, e.e2);
    }

    void CollisionSystem::OnTriggerEnter(const TriggerEnterEvent& e) {
        TMT_INFO << "Trigger Enter: " << (uint32_t)e.e1 << ", " << (uint32_t)e.e2;

        auto callback = e.reg->try_get<OnTriggerComponent>(e.e1);
        if (callback && callback->enter)
            callback->enter(e, e.e1);

        callback = e.reg->try_get<OnTriggerComponent>(e.e2);
        if (callback && callback->enter)
            callback->enter(e, e.e2);
    }

    void CollisionSystem::OnTriggerStay(const TriggerStayEvent& e) {
        //        TMT_INFO << "Trigger Stay";

        auto callback = e.reg->try_get<OnTriggerComponent>(e.e1);
        if (callback && callback->stay)
            callback->stay(e, e.e1);

        callback = e.reg->try_get<OnTriggerComponent>(e.e2);
        if (callback && callback->stay)
            callback->stay(e, e.e2);
    }

    void CollisionSystem::OnTriggerExit(const TriggerExitEvent& e) {
        TMT_INFO << "Trigger Exit : " << (uint32_t)e.e1 << ", " << (uint32_t)e.e2;

        auto callback = e.reg->try_get<OnTriggerComponent>(e.e1);
        if (callback && callback->exit)
            callback->exit(e, e.e1);

        callback = e.reg->try_get<OnTriggerComponent>(e.e2);
        if (callback && callback->exit)
            callback->exit(e, e.e2);
    }
}
