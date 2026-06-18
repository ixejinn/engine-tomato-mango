#ifndef MANGO_EPA_H
#define MANGO_EPA_H

#include <glm/vec3.hpp>
#include <vector>
#include <optional>
#include "Containers/UnorderedPair.h"
#include "ECS/Components/Collision.h"
#include "Collision/CollisionInfo.h"

namespace tomato {
    class EPA {
    public:
        struct Plain {
            Plain(const std::vector<glm::vec3>& points, uint32_t idx0, uint32_t idx1, uint32_t idx2);

            UnorderedPair<uint32_t> edges[3]{}; /// saves points indices.
            glm::vec3 normal{};
            float distance;                     /// from origin.
        };

        static std::optional<CollisionInfo> GetPenetrationInfo(std::vector<glm::vec3>& simplex,
                 const ColliderComponent& col1, const ColliderComponent& col2,
                 TransformComponent& trf1, TransformComponent& trf2);
    };
}

#endif //MANGO_EPA_H
