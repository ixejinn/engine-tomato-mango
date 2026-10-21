#ifndef MANGO_GJKRESULT_H
#define MANGO_GJKRESULT_H

#include <glm/vec3.hpp>
#include <vector>

namespace tomato
{
    struct DistanceResult
    {
        glm::vec3 normal;
        float distance;
        float maxDistSq{-1.f};
    };
}

#endif //MANGO_GJKRESULT_H