#ifndef MANGO_EPA_H
#define MANGO_EPA_H

#include <glm/vec3.hpp>
#include <vector>
#include <optional>
#include "Collision/Narrow/GJK/GJKResult.h"

namespace tomato
{
    namespace EPA
    {
        struct Plane
        {
            /**
             * idx0, idx1, idx2 counter-clockwise
             */
            Plane(uint32_t idx0, uint32_t idx1, uint32_t idx2, const glm::vec3& normal, float distance)
                : normal(normal), distance(distance)
            {
                edges[0] = {idx0, idx1};
                edges[1] = {idx1, idx2};
                edges[2] = {idx2, idx0};
            }

            std::pair<uint32_t, uint32_t> edges[3]; /// saves points indices. (counter-clockwise)
            glm::vec3 normal;
            float distance;                         /// from origin.
        };

        bool IsOffPlane(const glm::vec3& p, const glm::vec3& normal, const glm::vec3& planeP);
    }

    std::optional<DistanceResult> RunEPA(
            std::vector<glm::vec3>& points,
            const ColliderComponent& col1, const TransformComponent& trf1,
            const ColliderComponent& col2, const TransformComponent& trf2);
}

#endif //MANGO_EPA_H
