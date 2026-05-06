#include <entt/entt.hpp>
#include "ECS/Systems/CollisionSystem.h"
#include "ECS/Components/Collision.h"
#include "ECS/Components/Transform.h"
#include "ECS/SystemUpdateContexts.h"
#include "Collision/CollisionEvent.h"
#include "Collision/Broad/SAP.h"
#include "Collision/Narrow/GJK.h"
#include "Event/EventDispatcher.h"
#include "Utils/Logger.h"
#include "Utils/RegistryEntry.h"
REGISTER_SYSTEM(tomato::SystemPhase::Physics, CollisionSystem)

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
        int repeat = 1;
        while (repeat--) {
            candidates_.clear();

            BroadCheck(simCtx.registry);
            NarrowCheck(simCtx.registry);
        }
    }

    void CollisionSystem::BroadCheck(entt::registry &reg) {
        auto group = reg.group<ColliderComponent>();

        for (auto [e, col] : group.each())
        {
            if (col.aabbDirty)
                // Update AABB min, max
                SetAABB(col, reg.get<TransformComponent>(e));
        }

        broadPhase_->CheckBroadCollision(reg, candidates_);
    }

    void CollisionSystem::NarrowCheck(entt::registry& reg) {
        for (const auto& candidate : candidates_) {
            auto& col1 = reg.get<ColliderComponent>(candidate.a);
            auto& col2 = reg.get<ColliderComponent>(candidate.b);

            auto& trf1 = reg.get<TransformComponent>(candidate.a);
            auto& trf2 = reg.get<TransformComponent>(candidate.b);

            if (auto result = narrowPhase_->CheckNarrowCollision(col1, trf1, col2, trf2)) {
                // Collision detected
                auto& collisionInfo = *result;

                if (!collisionPairs_.contains(candidate)) {
                    // Enter
                    if (!collisionInfo)
                        EventDispatcher::GetInstance().Enqueue(TriggerEnterEvent{candidate.a, candidate.b, &reg});
                    else
                        EventDispatcher::GetInstance().Enqueue(CollisionEnterEvent{candidate.a, candidate.b, &reg, collisionInfo->normal, collisionInfo->depth});
                }
                else {
                    // Stay
                    if (!collisionInfo)
                        EventDispatcher::GetInstance().Enqueue(TriggerStayEvent{candidate.a, candidate.b, &reg});
                    else
                        EventDispatcher::GetInstance().Enqueue(CollisionStayEvent{candidate.a, candidate.b, &reg, collisionInfo->normal, collisionInfo->depth});
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

    void CollisionSystem::SetAABB(ColliderComponent& col, const TransformComponent& trf) {
        const auto& wPos = trf.GetPosition();

        switch (col.type)
        {
            case ColliderType::Sphere:
            {
                glm::vec3 radius{col.halfExtents.x};

                col.max = wPos + col.position + radius;
                col.min = wPos + col.position - radius;
            }
                break;
            default:
            {
                auto R = glm::toMat4(trf.GetQuaternion());

                glm::vec3 aabbHalfExtents
                {
                    glm::abs(R[0][0]) * col.halfExtents.x + glm::abs(R[1][0]) * col.halfExtents.y + glm::abs(R[2][0]) * col.halfExtents.z,
                    glm::abs(R[0][1]) * col.halfExtents.x + glm::abs(R[1][1]) * col.halfExtents.y + glm::abs(R[2][1]) * col.halfExtents.z,
                    glm::abs(R[0][2]) * col.halfExtents.x + glm::abs(R[1][2]) * col.halfExtents.y + glm::abs(R[2][2]) * col.halfExtents.z
                };

                auto wOffset = glm::vec3(R * glm::vec4(col.position, 1.f));
                col.max = wPos + wOffset + aabbHalfExtents;
                col.min = wPos + wOffset - aabbHalfExtents;
            }
                break;
        }
    }

    void CollisionSystem::OnCollisionEnter(const CollisionEnterEvent &e) {
        TMT_INFO << "Collision Enter: " << (uint32_t)e.e1 << ", " << (uint32_t)e.e2 <<
            " (" << e.depth << " : " << e.normal.x << "," << e.normal.y << "," << e.normal.z << ")";

        // TODO: 충돌 보정
        auto& trf1 = e.reg->get<TransformComponent>(e.e1);
        trf1.SetPosition(trf1.GetPosition() - e.normal * (e.depth + 0.01f));

        auto callback = e.reg->try_get<OnCollisionComponent>(e.e1);
        if (callback && callback->enter)
            callback->enter(e, e.e1);

        callback = e.reg->try_get<OnCollisionComponent>(e.e2);
        if (callback && callback->enter)
            callback->enter(e, e.e2);
    }

    void CollisionSystem::OnCollisionStay(const CollisionStayEvent& e) {
//        TMT_INFO << "Collision Stay";

        // TODO: 충돌 보정
        auto& trf1 = e.reg->get<TransformComponent>(e.e1);
        trf1.SetPosition(trf1.GetPosition() - e.normal * (e.depth + 0.01f));

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
        // TMT_INFO << "Trigger Enter: " << (uint32_t)e.e1 << ", " << (uint32_t)e.e2;

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
        // TMT_INFO << "Trigger Exit : " << (uint32_t)e.e1 << ", " << (uint32_t)e.e2;

        auto callback = e.reg->try_get<OnTriggerComponent>(e.e1);
        if (callback && callback->exit)
            callback->exit(e, e.e1);

        callback = e.reg->try_get<OnTriggerComponent>(e.e2);
        if (callback && callback->exit)
            callback->exit(e, e.e2);
    }
}
