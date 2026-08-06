#ifndef MANGO_EPA_H
#define MANGO_EPA_H

#include <glm/vec3.hpp>
#include <vector>
#include <optional>
#include "Containers/UnorderedPair.h"
#include "Collision/Narrow/GJK/GJKResult.h"

namespace tomato
{
    struct EPAPlain
    {
        EPAPlain(
            const std::vector<glm::vec3>& points,
            uint32_t idx0, uint32_t idx1, uint32_t idx2);

        UnorderedPair<uint32_t> edges[3]{}; /// saves points indices.
        glm::vec3 normal{};
        float distance;                     /// from origin.
    };

    std::optional<DistanceResult> RunEPA(
            std::vector<glm::vec3>& points,
            const ColliderComponent& col1, TransformComponent& trf1,
            const ColliderComponent& col2, TransformComponent& trf2);
}

#endif //MANGO_EPA_H
