#ifndef MANGO_NARROWPHASE_H
#define MANGO_NARROWPHASE_H

#include <optional>
#include <entt/fwd.hpp>
#include "Collision/CollisionResult.h"
#include "Collision/CollisionPair.h"

namespace tomato {
    /**
     * @brief Base class for the narrow-phase collision detection.
     */
    class NarrowPhase {
    public:
        virtual ~NarrowPhase() = default;

        /**
         * @brief Check whether the given pair intersects.
         * @param e1 Collider entity
         * @param e2 Collider entity
         * @return A CollisionInfo if a collision occurs. Otherwise, std::nullopt.
         */
        virtual std::optional<CollisionResult> CheckIntersection(
            entt::registry& reg, const CollisionPair& pair) = 0;
    };
}

#endif //MANGO_NARROWPHASE_H