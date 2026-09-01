#ifndef MANGO_BROADPHASE_H
#define MANGO_BROADPHASE_H

#include "Collision/CollisionFwd.h"
#include "Collision/CollisionLayerMatrix.h"

namespace tomato
{
    class BroadPhase
    {
    public:
        virtual ~BroadPhase() = default;

        virtual void FindContactPairCandidates(entt::registry& reg, std::vector<ContactPair>& candidates) = 0;

    protected:
        inline static CollisionLayerMatrix layerMatrix_;

        static bool CanCollide(
            entt::registry& reg,
            entt::entity a, entt::entity b,
            CollisionLayer layerA, CollisionLayer layerB);
    };
}

#endif //MANGO_BROADPHASE_H