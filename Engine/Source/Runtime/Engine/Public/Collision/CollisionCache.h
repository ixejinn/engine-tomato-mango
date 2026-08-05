#ifndef MANGO_COLLISIONCACHE_H
#define MANGO_COLLISIONCACHE_H

#include <glm/vec3.hpp>

namespace tomato
{
    struct ContactCache
    {
        glm::vec3 normal;
        int curr;
    };
}

#endif //MANGO_COLLISIONCACHE_H
