#include "Collision/Broad/BroadPhase.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/Collision.h"
#include "ECS/Components/Rigidbody.h"
#include "ECS/Entity/Hierarchy.h"

namespace tomato
{
    bool BroadPhase::CanCollide(
            entt::registry& reg,
            entt::entity a, entt::entity b,
            CollisionLayer layerA, CollisionLayer layerB)
    {
        entt::entity rootA = GetRootEntity(reg, a);
        entt::entity rootB = GetRootEntity(reg, b);

        return rootA != rootB
            && layerMatrix_.CanCollide(layerA, layerB)
            && (reg.try_get<VelocityComponent>(rootA) || reg.try_get<VelocityComponent>(rootB));
    }
}
