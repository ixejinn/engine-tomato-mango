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
        std::optional<CollisionInfo> DetectCollision(
            entt::registry& reg, entt::entity e1, entt::entity e2) override;

        static glm::vec3 GetSupportPoint(
            const glm::vec3& worldDir,
            const ColliderComponent& col1, TransformComponent& trf1,
            const ColliderComponent& col2, TransformComponent& trf2);

    private:
        static glm::vec3 Support(
            const glm::vec3& worldDir,
            const ColliderComponent& col, TransformComponent& trf);

        static std::optional<glm::vec3> FindClosestPointOnSimplex(std::vector<glm::vec3>& simplex);
        static glm::vec3 FindClosestPointOnTriangle(std::vector<glm::vec3>& simplex);

        static glm::vec3 ClosestPtPointTriangle(
            const glm::vec3& p,
            const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
        static int PointOutsideOfPlane(
            const glm::vec3& p,
            const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
        static int PointOutsideOfPlane(
            const glm::vec3& p,
            const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d);

        static bool AddSimplexPoint(
                std::vector<glm::vec3>& simplex,
                const ColliderComponent& col1, TransformComponent& trf1,
                const ColliderComponent& col2, TransformComponent& trf2);

        static bool VoronoiRegion(std::vector<glm::vec3>& simplex);

        using SupportFunc = std::function<glm::vec3(const glm::vec3& dir, const ColliderComponent& col)>;
        static EnumArray<ColliderType, SupportFunc> supportFunctions_;
    };
}

#endif //MANGO_GJK_H
