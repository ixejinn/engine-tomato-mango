#ifndef MANGO_EPA_H
#define MANGO_EPA_H

#include <glm/vec3.hpp>
#include <vector>
#include <optional>
#include "Containers/UnorderedPair.h"
#include "ECS/Components/Collision.h"
#include "Collision/CollisionInfo.h"

namespace tomato {
    struct EPAPlain {
        EPAPlain(const std::vector<glm::vec3>& simplex, uint32_t idx0, uint32_t idx1, uint32_t idx2);
        EPAPlain(const std::vector<glm::vec3>& simplex, uint32_t idx0, uint32_t idx1);

        UnorderedPair<uint32_t> edgeIndices[3]{};
        glm::vec3 normal{};
        float distance{}; // from origin

    private:
        void SetPlain(const std::vector<glm::vec3>& simplex, uint32_t idx0, uint32_t idx1, uint32_t idx2);
    };

    class EPA {
    public:
        static std::optional<CollisionInfo> GetNormalDepth(std::vector<glm::vec3>& simplex,
                 const ColliderComponent& col1, const ColliderComponent& col2,
                 TransformComponent& trf1, TransformComponent& trf2);
    };
}

#endif //MANGO_EPA_H
