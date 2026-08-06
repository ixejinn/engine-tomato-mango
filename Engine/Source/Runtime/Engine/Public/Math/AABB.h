#ifndef MANGO_AABB_H
#define MANGO_AABB_H

#include <glm/glm.hpp>
namespace tomato
{
    struct AABB
    {
        glm::vec3 min;
        glm::vec3 max;
    };

}
#endif // !MANGO_AABB_H
