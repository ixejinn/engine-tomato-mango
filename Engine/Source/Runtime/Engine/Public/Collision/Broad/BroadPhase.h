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
    };
}

#endif //MANGO_BROADPHASE_H