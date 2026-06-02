#ifndef MANGO_BROADPHASE_H
#define MANGO_BROADPHASE_H

#include "Collision/CollisionFwd.h"
#include "Collision/CollisionLayerMatrix.h"
#include "ECS/PhysCompFwd.h"

namespace tomato {
    class BroadPhase {
    public:
        virtual ~BroadPhase() = default;

        virtual void DetectCollision(entt::registry& reg, std::vector<CollisionPair>& candidates) = 0;

    protected:
        inline static CollisionLayerMatrix layerMatrix_;

        static bool CheckAABBAxisX(ColliderComponent& col1, ColliderComponent& col2);
        static bool CheckAABBAxisY(ColliderComponent& col1, ColliderComponent& col2);
        static bool CheckAABBAxisZ(ColliderComponent& col1, ColliderComponent& col2);

        static bool CanCollide(
            entt::registry& reg, entt::entity e1, entt::entity e2,
            ColliderComponent& col1, ColliderComponent& col2);
    };
}

#endif //MANGO_BROADPHASE_H