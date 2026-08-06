#ifndef MANGO_GJKRESULT_H
#define MANGO_GJKRESULT_H

#include <glm/vec3.hpp>

namespace tomato
{
    struct DistanceResult
    {
        glm::vec3 normal;
        float distance;
    };
}

#endif //MANGO_GJKRESULT_H