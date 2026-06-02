#include "Collision/Broad/BroadPhase.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/Collision.h"

namespace tomato {
    bool BroadPhase::CheckAABBAxisX(ColliderComponent& col1, ColliderComponent& col2) {
        return col1.max.x >= col2.min.x && col1.min.x <= col2.max.x;
    }
    bool BroadPhase::CheckAABBAxisY(ColliderComponent& col1, ColliderComponent& col2) {
        return col1.max.y >= col2.min.y && col1.min.y <= col2.max.y;
    }
    bool BroadPhase::CheckAABBAxisZ(ColliderComponent& col1, ColliderComponent& col2) {
        return col1.max.z >= col2.min.z && col1.min.z <= col2.max.z;
    }

    bool BroadPhase::CanCollide(
            entt::registry& reg, entt::entity e1, entt::entity e2,
            ColliderComponent& col1, ColliderComponent& col2) {
        return (GetRootEntity(reg, e1) != GetRootEntity(reg, e2)
            || layerMatrix_.CanCollide(col1.layer, col2.layer));
    }
}