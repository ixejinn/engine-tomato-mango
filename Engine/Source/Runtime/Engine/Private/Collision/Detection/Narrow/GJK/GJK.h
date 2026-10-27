#ifndef MANGO_GJK_H
#define MANGO_GJK_H

#include <glm/vec3.hpp>
#include "Collision/Detection/Narrow/ShapeView.h"
#include "Collision/Detection/Narrow/GJK/GJKResult.h"
#include "Collision/Collider/ColliderTypes.h"
#include "Collision/ContactData.h"
#include "ECS/Forward/PhysCompFwd.h"
#include "Containers/EnumArray.h"
#include "Event/EventSignal.h"

namespace tomato
{
    class GJK
    {
    public:
        GJK() = delete;

        static bool Overlap(const ShapeView& a, const ShapeView& b);
        static std::optional<ContactData> Sweep(const ShapeView& a, const ShapeView& b, const glm::vec3& ray, bool reversed);

        // for EPA
        static glm::vec3 GetSupportPoint(
            const glm::vec3& worldDir,
            const ColliderComponent& col1, const TransformComponent& trf1,
            const ColliderComponent& col2, const TransformComponent& trf2);

    private:
        static std::optional<DistanceResult> Distance(const ShapeView& a, const ShapeView& b);
        static std::optional<ContactData>    Raycast(const ShapeView& a, const ShapeView& b, const glm::vec3& ray, bool reversed);

        static bool IsTouching(const DistanceResult& result);

        static glm::vec3 Support(
            const glm::vec3& worldDir,
            const ColliderComponent& col, const TransformComponent& trf);

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

        using SupportFunc = std::function<glm::vec3(const glm::vec3& dir, const TransformComponent& trf)>;
        static EnumArray<ColliderType, SupportFunc> supportFunctions_;
    };
}

#endif //MANGO_GJK_H
