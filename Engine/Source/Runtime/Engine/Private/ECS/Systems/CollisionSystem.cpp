#include <entt/entt.hpp>
#include "ECS/Systems/CollisionSystem.h"
#include "ECS/Components/Collision.h"
#include "ECS/Components/Transform.h"
#include "ECS/SystemUpdateContexts.h"
#include "Collision/CollisionEvent.h"
#include "Event/EventDispatcher.h"
#include "Utils/Logger.h"
#include "Utils/RegistryEntry.h"
REGISTER_SYSTEM(tomato::SystemPhase::Physics, CollisionSystem)

namespace tomato {
    CollisionSystem::CollisionSystem() {
        EventDispatcher::GetInstance().Connect<CollisionEnterEvent, &CollisionSystem::OnCollisionEnter>();
        EventDispatcher::GetInstance().Connect<CollisionStayEvent, &CollisionSystem::OnCollisionStay>();
        EventDispatcher::GetInstance().Connect<CollisionExitEvent, &CollisionSystem::OnCollisionExit>();

        EventDispatcher::GetInstance().Connect<TriggerEnterEvent, &CollisionSystem::OnTriggerEnter>();
        EventDispatcher::GetInstance().Connect<TriggerStayEvent, &CollisionSystem::OnTriggerStay>();
        EventDispatcher::GetInstance().Connect<TriggerExitEvent, &CollisionSystem::OnTriggerExit>();
    }

    void CollisionSystem::Update(SimContext& simCtx) {
        candidates_.clear();

        BroadPhase(simCtx.registry);
        NarrowPhase(simCtx.registry);
    }

    void CollisionSystem::BroadPhase(entt::registry &reg) {
        auto group = reg.group<ColliderComponent>();

        for (auto [e, col] : group.each())
        {
            if (col.aabbDirty)
                // Update AABB min, max
                SetAABB(col, reg.get<TransformComponent>(e));
        }

        SAP(reg);
    }

    void CollisionSystem::NarrowPhase(entt::registry& reg) {
        for (const auto& candidate : candidates_) {
            auto& col1 = reg.get<ColliderComponent>(candidate.a);
            auto& col2 = reg.get<ColliderComponent>(candidate.b);

            auto& trf1 = reg.get<TransformComponent>(candidate.a);
            auto& trf2 = reg.get<TransformComponent>(candidate.b);

            if (narrowCheckFunc_(col1, trf1, col2, trf2)) {
                // Collision

                if (collisionPairs_.find(candidate) == collisionPairs_.end()) {
                    // Enter
                    if (col1.isTrigger || col2.isTrigger)
                        EventDispatcher::GetInstance().Enqueue(TriggerEnterEvent{candidate.a, candidate.b, &reg});
                    else
                        EventDispatcher::GetInstance().Enqueue(CollisionEnterEvent{candidate.a, candidate.b, &reg});
                }
                else {
                    // Stay
                    if (col1.isTrigger || col2.isTrigger)
                        EventDispatcher::GetInstance().Enqueue(TriggerStayEvent{candidate.a, candidate.b, &reg});
                    else
                        EventDispatcher::GetInstance().Enqueue(CollisionStayEvent{candidate.a, candidate.b, &reg});
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

    void CollisionSystem::SAP(entt::registry& reg) {
        auto group = reg.group<ColliderComponent>();

        // Sort by AABB.min.x for x-axis SAP
        group.sort<ColliderComponent>([](const auto& l, const auto& r)
                                      { return l.min.x < r.min.x; });

        // Active list contains AABBs that are currently open on the sweep axis.
        std::list<entt::entity> active;
        float activeMaxX = std::numeric_limits<float>::lowest();

        for (auto [e, col] : group.each())
        {
            // Initialize active list
            if (activeMaxX < col.min.x)
            {
                active.clear();

                active.push_back(e);
                activeMaxX = col.max.x;
            }
            else
            {
                for (auto it = active.begin(); it != active.end();)
                {
                    auto& colAct = reg.get<ColliderComponent>(*it);

                    // If active AABB.max < current AABB.min
                    // active AABB does not overlap on the sweep axis and cannot collide.
                    if (colAct.max.x < col.min.x)
                    {
                        active.erase(it++);
                        continue;
                    }

                    // Check collision layer
                    if (!layerMatrix_.CanCollide(col.layer, colAct.layer))
                    {
                        ++it;
                        continue;
                    }

                    // Check AABB
                    if (colAct.max.y < col.min.y || col.max.y < colAct.min.y)
                    {
                        ++it;
                        continue;
                    }
                    if (colAct.max.z < col.min.z || col.max.z < colAct.min.z)
                    {
                        ++it;
                        continue;
                    }

                    candidates_.emplace_back(e, *it);
                    ++it;
                }

                active.push_back(e);
                activeMaxX = std::max(activeMaxX, col.max.x);
            }
        }
    }

    void CollisionSystem::OnCollisionEnter(const CollisionEnterEvent &e) {
        // TMT_INFO << "Collision Enter: " << (uint32_t)e.e1 << ", " << (uint32_t)e.e2;

        // TODO: 충돌 보정

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
        // TMT_INFO << "Collision Exit : " << (uint32_t)e.e1 << ", " << (uint32_t)e.e2;

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
