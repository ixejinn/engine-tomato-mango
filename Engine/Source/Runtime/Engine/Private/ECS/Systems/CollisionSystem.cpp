#include <entt/entt.hpp>
#include "ECS/Systems/CollisionSystem.h"
#include "ECS/Components/Collision.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Render.h"
#include "ECS/SystemUpdateContexts.h"
#include "Collision/ColliderSupport.h"
#include "Collision/CollisionEvent.h"
#include "Event/EventDispatcher.h"
#include "Utils/Logger.h"
#include "Utils/RegistryEntry.h"
REGISTER_SYSTEM(tomato::SystemPhase::Physics, CollisionSystem)

namespace tomato {
    CollisionSystem::CollisionSystem() {
        // Registry support function per collider
        supportFunctions_[ColliderType::Cube] = support::Cube;
        supportFunctions_[ColliderType::Sphere] = support::Sphere;
        supportFunctions_[ColliderType::Capsule] = support::Capsule;

        EventDispatcher::GetInstance().Connect<CollisionEnterEvent, &CollisionSystem::OnCollisionEnter>();
        EventDispatcher::GetInstance().Connect<CollisionStayEvent, &CollisionSystem::OnCollisionStay>();
        EventDispatcher::GetInstance().Connect<CollisionExitEvent, &CollisionSystem::OnCollisionExit>();

        EventDispatcher::GetInstance().Connect<TriggerEnterEvent, &CollisionSystem::OnTriggerEnter>();
        EventDispatcher::GetInstance().Connect<TriggerStayEvent, &CollisionSystem::OnTriggerStay>();
        EventDispatcher::GetInstance().Connect<TriggerExitEvent, &CollisionSystem::OnTriggerExit>();
    }

    void CollisionSystem::Update(SimContext& simCtx) {
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
            auto& col1 = reg.get<ColliderComponent>(candidate.e1);
            auto& col2 = reg.get<ColliderComponent>(candidate.e2);

            auto& trf1 = reg.get<TransformComponent>(candidate.e1);
            auto& trf2 = reg.get<TransformComponent>(candidate.e2);

            if (GJK(col1, trf1, col2, trf2)) {
                // Collision

                if (collisionPairs_.find(candidate) == collisionPairs_.end()) {
                    // Enter
                    if (col1.isTrigger || col2.isTrigger)
                        EventDispatcher::GetInstance().Enqueue(TriggerEnterEvent{candidate.e1, candidate.e2, &reg});
                    else
                        EventDispatcher::GetInstance().Enqueue(CollisionEnterEvent{candidate.e1, candidate.e2, &reg});
                }
                else {
                    // Stay
                    if (col1.isTrigger || col2.isTrigger)
                        EventDispatcher::GetInstance().Enqueue(TriggerStayEvent{candidate.e1, candidate.e2, &reg});
                    else
                        EventDispatcher::GetInstance().Enqueue(CollisionStayEvent{candidate.e1, candidate.e2, &reg});
                }

                collisionPairs_[candidate] = true;
            }
        }

        for (auto it = collisionPairs_.begin(); it != collisionPairs_.end(); ) {
            if (!it->second) {
                // Exit
                auto& col1 = reg.get<ColliderComponent>(it->first.e1);
                auto& col2 = reg.get<ColliderComponent>(it->first.e2);

                if (col1.isTrigger || col2.isTrigger)
                    EventDispatcher::GetInstance().Enqueue(TriggerExitEvent{it->first.e1, it->first.e2, &reg});
                else
                    EventDispatcher::GetInstance().Enqueue(CollisionExitEvent{it->first.e1, it->first.e2, &reg});

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
//                {
//                    glm::abs(R[0][0]) * col.halfExtents.x + glm::abs(R[1][0]) * col.halfExtents.y + glm::abs(R[2][0]) * col.halfExtents.z,
//                    glm::abs(R[0][1]) * col.halfExtents.x + glm::abs(R[1][1]) * col.halfExtents.y + glm::abs(R[2][1]) * col.halfExtents.z,
//                    glm::abs(R[0][2]) * col.halfExtents.x + glm::abs(R[1][2]) * col.halfExtents.y + glm::abs(R[2][2]) * col.halfExtents.z
//                };
                = col.halfExtents;

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

    bool CollisionSystem::GJK(
            const ColliderComponent& col1, const TransformComponent& trf1,
            const ColliderComponent& col2, const TransformComponent& trf2) {
        std::vector<glm::vec3> simplex;
        simplex.reserve(4);

        while (true)
        {
            if (!AddSimplexPoint(simplex, col1, trf1, col2, trf2))
                return false;   // 비충돌 종료

            if (VoronoiRegion(simplex))
                return true;    // 충돌 종료
        }
    }

    glm::vec3 CollisionSystem::Support(
            const glm::vec3& worldDir,
            const ColliderComponent& col, const TransformComponent& trf) {
        const auto R = glm::toMat4(trf.GetQuaternion());
        const glm::vec4 localDir = glm::transpose(R) * glm::vec4(worldDir, 0.f);

        const auto localSupportP = supportFunctions_[col.type](localDir, col);
        return glm::vec3{trf.GetTransformMatrix() * glm::vec4(col.position + localSupportP, 1.f)};
    }

    bool CollisionSystem::AddSimplexPoint(
            std::vector<glm::vec3>& simplex,
            const ColliderComponent& col1, const TransformComponent& trf1,
            const ColliderComponent& col2, const TransformComponent& trf2) {
        glm::vec3 dir;
        auto simplexSize = simplex.size();

        switch (simplexSize)
        {
            case 0:
            {
                glm::vec3 wPosCol1 = trf1.GetTransformMatrix() * glm::vec4(col1.position, 1.f);
                glm::vec3 wPosCol2 = trf2.GetTransformMatrix() * glm::vec4(col2.position, 1.f);

                simplex.push_back(Support(wPosCol2 - wPosCol1, col1, trf1) - Support(wPosCol1 - wPosCol2, col2, trf2));
            }
                break;

            case 1:
                dir = -simplex[0];
                simplex.push_back(Support(dir, col1, trf1) - Support(-dir, col2, trf2));

                if (glm::dot(dir, simplex[1]) < 0)
                    return false;   // 심플렉스가 원점을 포함할 수 없음
                break;

            case 2:
            {
                const auto ab = simplex[1] - simplex[0];
                const auto ao = -simplex[0];
                dir = glm::cross(glm::cross(ab, ao), ab);
                simplex.push_back(Support(dir, col1, trf1) - Support(-dir, col2, trf2));

                if (glm::dot(dir, simplex[2]) < 0)
                    return false;
            }
                break;

            case 3:
                dir = GetOrientedNormal(glm::vec3{0.f}, simplex[0], simplex[1], simplex[2]);
                simplex.push_back(Support(dir, col1, trf1) - Support(-dir, col2, trf2));

                if (glm::dot(dir, simplex[3]) < 0)
                    return false;
                break;

            default:
                TMT_WARN << "Incorrect simplex points.";
        }

        return true;
    }

    bool CollisionSystem::VoronoiRegion(std::vector<glm::vec3> &simplex) {
        auto simplexSize = simplex.size();
        switch (simplexSize)
        {
            case 1:
                // if (simplex[0] == Vector3{0.f}) return true;
                return false;

            case 2:
            {
                const auto ao = -simplex[0];
                const auto ab = simplex[1] - simplex[0];

                const auto dotV = glm::dot(ao, ab);
                if (dotV < 0)
                    simplex.pop_back();
                else if (dotV > glm::dot(ab, ab))
                    simplex.erase(simplex.begin());

                return false;
            }

            case 3:
            {
                const auto ab = simplex[1] - simplex[0];
                const auto bc = simplex[2] - simplex[1];
                const auto ac = simplex[2] - simplex[0];
                const auto ao = -simplex[0];
                const auto bo = -simplex[1];

                const auto normal = glm::cross(ac, ab);

                if (glm::dot(ao, glm::cross(normal, ab)) > 0)
                {
                    simplex.pop_back();
                    return false;
                }

                if (glm::dot(bo, glm::cross(normal, bc)) > 0)
                {
                    simplex.erase(simplex.begin());
                    return false;
                }

                if (glm::dot(ao, glm::cross(normal, -ac)) > 0)
                {
                    simplex.erase(++simplex.begin());
                    return false;
                }

                // 2차원이면 return true;
                return false;
            }

            case 4:
            {
                const auto lo = -simplex[3];

                if (glm::dot(-GetOrientedNormal(simplex[1], simplex[3], simplex[0], simplex[2]), lo) > 0)
                {
                    simplex.erase(++simplex.begin());
                    return false;
                }
                if (glm::dot(-GetOrientedNormal(simplex[0], simplex[3], simplex[2], simplex[1]), lo) > 0)
                {
                    simplex.erase(simplex.begin());
                    return false;
                }
                if (glm::dot(-GetOrientedNormal(simplex[2], simplex[3], simplex[1], simplex[0]), lo) > 0)
                {
                    simplex.erase(simplex.begin() + 2);
                    return false;
                }
                return true;
            }

            default:
                TMT_WARN << "Incorrect simplex size.";
                return false;
        }
    }

    glm::vec3 CollisionSystem::GetOrientedNormal(
        const glm::vec3 &refP,
        const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2) {
        const auto vec01 = p1 - p0;
        const auto vec02 = p2 - p0;
        auto normal = glm::cross(vec01, vec02);
        auto x = glm::dot(normal, (refP - p0));
        return (x > 0 ? normal : -normal);
    }

    void CollisionSystem::OnCollisionEnter(const CollisionEnterEvent &e) {
        TMT_INFO << "Collision Enter: " << (uint32_t)e.e1 << ", " << (uint32_t)e.e2;

        // 충돌 보정

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
