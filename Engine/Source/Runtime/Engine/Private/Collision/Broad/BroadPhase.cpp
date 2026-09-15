#include "Collision/Broad/BroadPhase.h"
#include "Collision/CollisionLayerMatrix.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/Collision.h"
#include "ECS/Components/Rigidbody.h"
#include "ECS/Entity/Hierarchy.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"

namespace tomato
{
    bool BroadPhase::CanCollide(
            entt::registry& reg,
            entt::entity a, entt::entity b,
            CollisionLayer layerA, CollisionLayer layerB)
    {
        entt::entity rootA = GetRootEntity(reg, a);
        entt::entity rootB = GetRootEntity(reg, b);
        if (rootA == rootB)
            return false;

        if (!reg.ctx().get<CollisionContext>().layerMtx->CanCollide(layerA, layerB))
            return false;

        if (!reg.try_get<VelocityComponent>(rootA) && !reg.try_get<VelocityComponent>(rootB))
            return false;

        return true;
    }
}
