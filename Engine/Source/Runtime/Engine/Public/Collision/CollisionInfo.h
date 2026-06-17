#ifndef MANGO_COLLISIONINFO_H
#define MANGO_COLLISIONINFO_H

#include <glm/vec3.hpp>

namespace tomato {
    struct CollisionInfo {
        glm::vec3 normal;
        float depth;
        float weight;
    };
}

#endif //MANGO_COLLISIONINFO_H