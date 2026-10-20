#include "Collision/Detection/Broad/BroadPhase.h"
#include "Collision/Layer/CollisionLayerMatrix.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/Collision.h"
#include "ECS/Components/Velocity.h"
#include "ECS/Components/PhysicsBody.h"
#include "ECS/Entity/Hierarchy.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"

namespace tomato
{
    bool BroadPhase::CanCollide(
            entt::registry& reg,
            entt::entity a, entt::entity b,
            const ColliderComponent& colA, const ColliderComponent& colB)
    {
        if (colA.trigger && colB.trigger)
            return false;

        if (!reg.ctx().get<CollisionContext>().layerMtx->CanCollide(colA.layer, colB.layer))
            return false;

        entt::entity rootA = GetRootEntity(reg, a);
        entt::entity rootB = GetRootEntity(reg, b);
        if (rootA == rootB)
            return false;

        auto* bodyA = reg.try_get<PhysicsBodyComponent>(rootA);
        auto* bodyB = reg.try_get<PhysicsBodyComponent>(rootB);
        if ((!bodyA || bodyA->inverseMass == 0) && (!bodyB || bodyB->inverseMass == 0))
            return false;

        return true;
    }
}
