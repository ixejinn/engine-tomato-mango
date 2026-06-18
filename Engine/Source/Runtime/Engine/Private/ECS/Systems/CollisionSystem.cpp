#include <entt/entt.hpp>
#include "ECS/Systems/CollisionSystem.h"

#include "SimulationConfig.h"
#include "ECS/Components/Collision.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/Movement.h"
#include "ECS/SystemUpdateContexts.h"
#include "Collision/CollisionEvent.h"
#include "Collision/CollisionPair.h"
#include "Collision/Broad/SAP.h"
#include "Collision/Narrow/GJK.h"
#include "ECS/Components/Rigidbody.h"
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

        EventDispatcher::GetInstance().Connect<PenetrationEvent, &CollisionSystem::OnPenetration>();
    }

    CollisionSystem::~CollisionSystem() = default;

    void CollisionSystem::Update(SimContext& simCtx) {
        DetectBroad(simCtx);
        DetectNarrow(simCtx);

        EventDispatcher::GetInstance().Update<CollisionEnterEvent>();   // TODO: 보정용으로 수정
        EventDispatcher::GetInstance().Update<CollisionStayEvent>();   // 보정용으로 수정
        EventDispatcher::GetInstance().Update<TriggerEnterEvent>();   // 보정용으로 수정
        EventDispatcher::GetInstance().Update<TriggerExitEvent>();   // 보정용으로 수정

        EventDispatcher::GetInstance().Update<PenetrationEvent>();
    }

    void CollisionSystem::DetectBroad(SimContext& simCtx) {
        candidates_.clear();

        // Update AABB min, max
        auto group = simCtx.registry.group<ColliderComponent>();
        for (auto [e, col] : group.each())
        {
            if (col.aabbDirty)
                SetAABB(simCtx.registry, e);
        }

        // Find collision candidates
        broadPhase_->FindCollisionCandidates(simCtx.registry, candidates_);
    }

    void CollisionSystem::DetectNarrow(SimContext& simCtx) {
        auto& eventDispatcher = EventDispatcher::GetInstance();

        for (const auto& candidate : candidates_) {
            if (auto result= narrowPhase_->CheckIntersection(simCtx.registry, candidate)) {
                if (!activePairs_.contains(candidate)) {
                    // New collision pair has been detected (ENTER state)
                    if (candidate.isTrigger)
                        eventDispatcher.Enqueue(
                            TriggerEnterEvent{candidate.entities.a, candidate.entities.b, &simCtx.registry});
                    else
                        eventDispatcher.Enqueue(
                            CollisionEnterEvent{candidate.entities.a, candidate.entities.b, &simCtx.registry, result.value()});
                }
                else {
                    // Existing collision pair continues to collide (STAY state)
                    if (candidate.isTrigger)
                        eventDispatcher.Enqueue(
                            TriggerStayEvent{candidate.entities.a, candidate.entities.b, &simCtx.registry});
                    else
                        eventDispatcher.Enqueue(
                            CollisionStayEvent{candidate.entities.a, candidate.entities.b, &simCtx.registry, result.value()});
                }

                activePairs_[candidate] = true;
            }
        }

        // Determine EXIT state
        for (auto it = activePairs_.begin(); it != activePairs_.end(); ) {
            if (!it->second) {
                // No collision occurred in this tick.
                if (it->first.isTrigger)
                    eventDispatcher.Enqueue(
                        TriggerExitEvent{it->first.entities.a, it->first.entities.b, &simCtx.registry});
                else
                    eventDispatcher.Enqueue(
                        CollisionExitEvent{it->first.entities.a, it->first.entities.b, &simCtx.registry});

                it = activePairs_.erase(it);
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
        if (const auto* velPtr = reg.try_get<VelocityComponent>(GetRootEntity(reg, e)))
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

    void CollisionSystem::ResolveCollision(entt::registry& reg, entt::entity e1, entt::entity e2, const CollisionResult& info) {
        constexpr float VELOCITY_EPSILON = 0.001f;

        entt::entity root1 = GetRootEntity(reg, e1);
        entt::entity root2 = GetRootEntity(reg, e2);

        auto& trfRoot1 = reg.get<TransformComponent>(root1);
        auto& trfRoot2 = reg.get<TransformComponent>(root2);

        if (auto* vel = reg.try_get<VelocityComponent>(root1)) {
            glm::vec3 remainingMove = (1 - info.toi * info.weight) * vel->velocity;

            trfRoot1.AddPosition((vel->velocity * FIXED_DELTA_TIME * info.toi - info.normal * COLLISION_SKIN) * info.weight);
            vel->velocity = remainingMove - glm::dot(remainingMove, info.normal) * info.normal;

            if (-VELOCITY_EPSILON < vel->velocity.x && vel->velocity.x < VELOCITY_EPSILON)
                vel->velocity.x = 0.f;
            if (-VELOCITY_EPSILON < vel->velocity.y && vel->velocity.y < VELOCITY_EPSILON)
                vel->velocity.y = 0.f;
            if (-VELOCITY_EPSILON < vel->velocity.z && vel->velocity.z < VELOCITY_EPSILON)
                vel->velocity.z = 0.f;
        }

        if (auto* vel = reg.try_get<VelocityComponent>(root2)) {
            float weight = 1 - info.weight;
            glm::vec3 remainingMove = (1 - info.toi * weight) * vel->velocity;

            trfRoot2.AddPosition((vel->velocity * FIXED_DELTA_TIME * info.toi + info.normal * COLLISION_SKIN) * weight);
            vel->velocity = remainingMove + glm::dot(remainingMove, -info.normal) * info.normal;

            if (-VELOCITY_EPSILON < vel->velocity.x && vel->velocity.x < VELOCITY_EPSILON)
                vel->velocity.x = 0.f;
            if (-VELOCITY_EPSILON < vel->velocity.y && vel->velocity.y < VELOCITY_EPSILON)
                vel->velocity.y = 0.f;
            if (-VELOCITY_EPSILON < vel->velocity.z && vel->velocity.z < VELOCITY_EPSILON)
                vel->velocity.z = 0.f;
        }
    }

    void CollisionSystem::OnPenetration(const PenetrationEvent& e) {
        constexpr float CORRECTION_SPEED = 5.f;

        auto& trfRoot1 = e.reg->get<TransformComponent>(GetRootEntity(*e.reg, e.e1));
        auto& trfRoot2 = e.reg->get<TransformComponent>(GetRootEntity(*e.reg, e.e2));

        trfRoot1.AddPosition(-e.info.normal * e.info.toi * e.info.weight * FIXED_DELTA_TIME * CORRECTION_SPEED);
        trfRoot2.AddPosition(e.info.normal * e.info.toi * (1 - e.info.weight) * FIXED_DELTA_TIME * CORRECTION_SPEED);
    }

    void CollisionSystem::OnCollisionEnter(const CollisionEnterEvent &e) {
//        TMT_INFO << "Collision Enter: " << (uint32_t)e.e1 << ", " << (uint32_t)e.e2;

        ResolveCollision(*e.reg, e.e1, e.e2, e.info);

        // 콜백 함수 호출
        auto callback = e.reg->try_get<OnCollisionComponent>(e.e1);
        if (callback && callback->enter)
            callback->enter(e, e.e1);

        callback = e.reg->try_get<OnCollisionComponent>(e.e2);
        if (callback && callback->enter)
            callback->enter(e, e.e2);
    }

    void CollisionSystem::OnCollisionStay(const CollisionStayEvent& e) {
//        TMT_INFO << "Collision Stay " << (uint32_t)e.e1 << ", " << (uint32_t)e.e2;

        ResolveCollision(*e.reg, e.e1, e.e2, e.info);

        auto callback = e.reg->try_get<OnCollisionComponent>(e.e1);
        if (callback && callback->stay)
            callback->stay(e, e.e1);

        callback = e.reg->try_get<OnCollisionComponent>(e.e2);
        if (callback && callback->stay)
            callback->stay(e, e.e2);
    }

    void CollisionSystem::OnCollisionExit(const CollisionExitEvent& e) {
//        TMT_INFO << "Collision Exit : " << (uint32_t)e.e1 << ", " << (uint32_t)e.e2;

        auto callback = e.reg->try_get<OnCollisionComponent>(e.e1);
        if (callback && callback->exit)
            callback->exit(e, e.e1);

        callback = e.reg->try_get<OnCollisionComponent>(e.e2);
        if (callback && callback->exit)
            callback->exit(e, e.e2);
    }

    void CollisionSystem::OnTriggerEnter(const TriggerEnterEvent& e) {
//        TMT_INFO << "Trigger Enter: " << (uint32_t)e.e1 << ", " << (uint32_t)e.e2;

        auto callback = e.reg->try_get<OnTriggerComponent>(e.e1);
        if (callback && callback->enter)
            callback->enter(e, e.e1);

        callback = e.reg->try_get<OnTriggerComponent>(e.e2);
        if (callback && callback->enter)
            callback->enter(e, e.e2);
    }

    void CollisionSystem::OnTriggerStay(const TriggerStayEvent& e) {
        // TMT_INFO << "Trigger Stay : " << (uint32_t)e.e1 << ", " << (uint32_t)e.e2;

        auto callback = e.reg->try_get<OnTriggerComponent>(e.e1);
        if (callback && callback->stay)
            callback->stay(e, e.e1);

        callback = e.reg->try_get<OnTriggerComponent>(e.e2);
        if (callback && callback->stay)
            callback->stay(e, e.e2);
    }

    void CollisionSystem::OnTriggerExit(const TriggerExitEvent& e) {
//        TMT_INFO << "Trigger Exit : " << (uint32_t)e.e1 << ", " << (uint32_t)e.e2;

        auto callback = e.reg->try_get<OnTriggerComponent>(e.e1);
        if (callback && callback->exit)
            callback->exit(e, e.e1);

        callback = e.reg->try_get<OnTriggerComponent>(e.e2);
        if (callback && callback->exit)
            callback->exit(e, e.e2);
    }
}
