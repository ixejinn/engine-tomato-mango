#ifndef MANGO_GJK_H
#define MANGO_GJK_H

#include <glm/vec3.hpp>
#include "ECS/PhysCompFwd.h"
#include "Collision/CollisionConstants.h"
#include "Containers/EnumArray.h"

namespace tomato {
    class GJK {
    public:
        static bool CheckCollision(
                const ColliderComponent& col1, const TransformComponent& trf1,
                const ColliderComponent& col2, const TransformComponent& trf2);

        static glm::vec3 Support(
                const glm::vec3& worldDir,
                const ColliderComponent& col, const TransformComponent& trf);

        static bool AddSimplexPoint(
                std::vector<glm::vec3>& simplex,
                const ColliderComponent& col1, const TransformComponent& trf1,
                const ColliderComponent& col2, const TransformComponent& trf2);

        static bool VoronoiRegion(std::vector<glm::vec3>& simplex);

        static glm::vec3 GetOrientedNormal(
                const glm::vec3& refP,
                const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2);

        using SupportFunc = std::function<glm::vec3(const glm::vec3& dir, const ColliderComponent& col)>;
        static EnumArray<ColliderType, SupportFunc> supportFunctions_;
    };
}

#endif //MANGO_GJK_H
