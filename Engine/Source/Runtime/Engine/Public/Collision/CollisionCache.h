#ifndef MANGO_COLLISIONCACHE_H
#define MANGO_COLLISIONCACHE_H

#include <glm/vec3.hpp>

namespace tomato
{
    struct CollisionCache
    {
        glm::vec3 normal;
        bool curr;
    };
}

#endif //MANGO_COLLISIONCACHE_H
