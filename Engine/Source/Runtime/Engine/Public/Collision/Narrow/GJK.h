#ifndef MANGO_GJK_H
#define MANGO_GJK_H

#include <glm/vec3.hpp>
#include "Collision/Narrow/NarrowPhase.h"
#include "Collision/CollisionConstants.h"
#include "Containers/EnumArray.h"
#include "Event/EventSignal.h"

namespace tomato {
    class GJK : public NarrowPhase {
    public:
        std::optional<std::optional<CollisionInfo>> CheckNarrowCollision(
                const ColliderComponent& col1, const TransformComponent& trf1,
                const ColliderComponent& col2, const TransformComponent& trf2) override;

        static glm::vec3 Support(
                const glm::vec3& worldDir,
                const ColliderComponent& col, const TransformComponent& trf);

        static bool AddSimplexPoint(
                std::vector<glm::vec3>& simplex,
                const ColliderComponent& col1, const TransformComponent& trf1,
                const ColliderComponent& col2, const TransformComponent& trf2);

        static bool VoronoiRegion(std::vector<glm::vec3>& simplex);

        using SupportFunc = std::function<glm::vec3(const glm::vec3& dir, const ColliderComponent& col)>;
        static EnumArray<ColliderType, SupportFunc> supportFunctions_;
    };
}

#endif //MANGO_GJK_H
