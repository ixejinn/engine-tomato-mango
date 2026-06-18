#ifndef MANGO_COLLISIONRESULT_H
#define MANGO_COLLISIONRESULT_H

#include <glm/vec3.hpp>

namespace tomato {
    /**
     * @brief Collision details for a CollisionPair(UnorderedPair<entt::entity>)
     */
    struct CollisionResult {
        /// Normal vector from CollisionPair.a to b
        glm::vec3 normal;

        /// Time of Impact
        float toi;

        /// Weight of CollisionPair.a
        float weight;
    };
}

#endif //MANGO_COLLISIONRESULT_H