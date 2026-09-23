#ifndef MANGO_BROADPHASE_H
#define MANGO_BROADPHASE_H

#include "Collision/CollisionFwd.h"
#include "ECS/Forward/PhysCompFwd.h"

namespace tomato
{
    class BroadPhase
    {
    public:
        virtual ~BroadPhase() = default;

        virtual void FindContactPairCandidates(entt::registry& reg, std::vector<ContactPair>& candidates) = 0;

    protected:
        static bool CanCollide(
            entt::registry& reg,
            entt::entity a, entt::entity b,
            const ColliderComponent& colA, const ColliderComponent& colB);
    };
}

#endif //MANGO_BROADPHASE_H