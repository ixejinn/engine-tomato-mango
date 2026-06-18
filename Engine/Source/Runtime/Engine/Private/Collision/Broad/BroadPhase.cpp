#include "Collision/BroadPhase.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/Collision.h"
#include "ECS/Components/Rigidbody.h"

namespace tomato {
    bool BroadPhase::CheckAABBAxisX(const ColliderComponent& col1, const ColliderComponent& col2) {
        return col1.max.x >= col2.min.x && col1.min.x <= col2.max.x;
    }
    bool BroadPhase::CheckAABBAxisY(const ColliderComponent& col1, const ColliderComponent& col2) {
        return col1.max.y >= col2.min.y && col1.min.y <= col2.max.y;
    }
    bool BroadPhase::CheckAABBAxisZ(const ColliderComponent& col1, const ColliderComponent& col2) {
        return col1.max.z >= col2.min.z && col1.min.z <= col2.max.z;
    }

    bool BroadPhase::CanCollide(
            entt::registry& reg, entt::entity e1, entt::entity e2,
            const ColliderComponent& col1, const ColliderComponent& col2) {
        const entt::entity root1 = GetRootEntity(reg, e1);
        const entt::entity root2 = GetRootEntity(reg, e2);
        return root1 != root2 &&
            layerMatrix_.CanCollide(col1.layer, col2.layer) &&
                (reg.try_get<VelocityComponent>(root1) || reg.try_get<VelocityComponent>(root2));
    }
}
