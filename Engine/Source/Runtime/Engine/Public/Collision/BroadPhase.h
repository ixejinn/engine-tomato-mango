#ifndef MANGO_BROADPHASE_H
#define MANGO_BROADPHASE_H

#include "Collision/CollisionFwd.h"
#include "Collision/CollisionLayerMatrix.h"
#include "Collision/CollisionPair.h"
#include "ECS/PhysCompFwd.h"

namespace tomato {
    /**
     * @brief Base class for the broad-phase collision detection.
     */
    class BroadPhase {
    public:
        virtual ~BroadPhase() = default;

        /**
         * @brief Finds all potential collision candidates from the registry.
         * @param candidates Broad-phase collision candidates for narrow-phase check.
         */
        virtual void FindCollisionCandidates(entt::registry& reg, std::vector<CollisionPair>& candidates) = 0;

    protected:
        static bool CheckAABBAxisX(const ColliderComponent& col1, const ColliderComponent& col2);
        static bool CheckAABBAxisY(const ColliderComponent& col1, const ColliderComponent& col2);
        static bool CheckAABBAxisZ(const ColliderComponent& col1, const ColliderComponent& col2);

        static bool CanCollide(
            entt::registry& reg, entt::entity e1, entt::entity e2,
            const ColliderComponent& col1, const ColliderComponent& col2);

    private:
        inline static CollisionLayerMatrix layerMatrix_;
    };
}

#endif //MANGO_BROADPHASE_H