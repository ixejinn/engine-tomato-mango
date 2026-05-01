#ifndef MANGO_EPA_H
#define MANGO_EPA_H

#include <glm/vec3.hpp>
#include <vector>
#include "Containers/UnorderedPair.h"
#include "ECS/Components/Collision.h"

namespace tomato {
    struct EPAEvent {
        EPAEvent(std::vector<glm::vec3>& spx,
                 const ColliderComponent& c1, const ColliderComponent& c2,
                 const TransformComponent& t1, const TransformComponent& t2)
        : simplex(spx), col1(c1), col2(c2), trf1(t1), trf2(t2) {}

        std::vector<glm::vec3>& simplex;

        const ColliderComponent& col1;
        const ColliderComponent& col2;
        const TransformComponent& trf1;
        const TransformComponent& trf2;
    };

    struct EPAPlain {
        EPAPlain(std::vector<glm::vec3>& simplex, size_t idx0, size_t idx1, size_t idx2);

        UnorderedPair<size_t> edges[3];
        glm::vec3 normal;
        float distance; // from origin
    };

    class EPA {
    public:
        static void GetNormalDepth(const EPAEvent& e);
    };
}

#endif //MANGO_EPA_H
