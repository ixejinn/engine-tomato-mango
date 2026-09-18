#include <unordered_set>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include "ECS/Components/Transform.h"
#include "ECS/Components/Collision.h"
#include "Collision/Narrow/GJK/EPA.h"
#include "Collision/Narrow/GJK/GJK.h"
#include "Collision/CollisionConfig.h"
#include "Containers/UnorderedPair.h"
#include "Math/Normal.h"
#include "Utils/Logger.h"

namespace tomato
{
    bool EPA::IsOffPlane(const glm::vec3& p, const glm::vec3& normal, const glm::vec3& planeP)
    {
        const glm::vec3 planeToP = p - planeP;
        const float dist = glm::dot(planeToP, normal);
        return dist * dist > RELATIVE_TOLERANCE_SQ * glm::length2(normal) * glm::length2(planeToP);
    }

    std::optional<DistanceResult> RunEPA(
        std::vector<glm::vec3>& points,
        const ColliderComponent& col1, const TransformComponent& trf1,
        const ColliderComponent& col2, const TransformComponent& trf2)
    {
        // std::cout << "========== EPA\n";

        //// Extend points to 3-simplex
        while (points.size() < 4)
        {
            switch (points.size())
            {
            case 1:
                points.emplace_back(GJK::GetSupportPoint(-points[0], col1, trf1, col2, trf2));
                break;
            case 2:
            {
                const auto ao = -points[0];
                const auto ab = points[1] - points[0];

                auto t = glm::dot(ao, ab);
                if (t <= 0) {
                    points.pop_back();
                    points.emplace_back(GJK::GetSupportPoint(-points[0], col1, trf1, col2, trf2));
                }
                else {
                    float denom = glm::length2(ab);

                    if (t >= denom) {
                        points.erase(points.begin());
                        points.emplace_back(GJK::GetSupportPoint(-points[0], col1, trf1, col2, trf2));
                    }
                    else {
                        t /= denom;
                        auto p = points[0] + t * ab;
                        points.emplace_back(GJK::GetSupportPoint(-p, col1, trf1, col2, trf2));
                    }
                }
            }
                break;
            case 3:
            {
                const glm::vec3 a = points[0];
                const glm::vec3 b = points[1];
                const glm::vec3 c = points[2];

                const glm::vec3 ab = b - a;
                const glm::vec3 ac = c - a;

                glm::vec3 normal = glm::cross(ab, ac);
                if (glm::dot(-a, normal) < 0)
                    normal = -normal;

                glm::vec3 supportP = GJK::GetSupportPoint(normal, col1, trf1, col2, trf2);
                if (!EPA::IsOffPlane(supportP, normal, a))
                    supportP = GJK::GetSupportPoint(-normal, col1, trf1, col2, trf2);
                if (!EPA::IsOffPlane(supportP, normal, a))
                {
                    TMT_WARN << "Degenerate 3-simplex";
                    return std::nullopt;
                }

                points.emplace_back(supportP);
            }
            break;
            }
        }

        // Sort simplex point for counter-clockwise
        const glm::vec3 n012 = glm::cross(points[1] - points[0], points[2] - points[0]);
        if (glm::dot(n012, points[3] - points[0]) > 0)
            std::swap(points[1], points[2]);

        //// Create polytope for EPA
        std::vector<EPA::Plane> polytope;

        const uint32_t faces[4][3]
        {
            {0, 1, 2},
            {0, 2, 3},
            {0, 3, 1},
            {1, 3, 2}
        };
        for (const auto& face : faces)
        {
            if (auto normal = GetNormal(points[face[0]], points[face[1]], points[face[2]]))
            {
                polytope.emplace_back(face[0], face[1], face[2],
                    normal.value(), glm::dot(normal.value(), points[face[0]]));
            }
            else
            {
                TMT_WARN << "Degenerate polytope plane";
                return std::nullopt;
            }
        }

        //// EPA
        float maxDistSq = 0.f;
        for (const glm::vec3& p : points)
            maxDistSq = std::max(maxDistSq, glm::length2(p));

        int iteration = 0;
        while (true)
        {
            EPA::Plane* nearest{nullptr};
            for (auto& plane : polytope)
            {
                if (!nearest || nearest->distance > plane.distance)
                    nearest = &plane;
            }

            if (!nearest)
            {
                TMT_ERR << "Incorrect polytope";
                return std::nullopt;
            }

            const glm::vec3 supportP = GJK::GetSupportPoint(nearest->normal, col1, trf1, col2, trf2);
            points.push_back(supportP);
            maxDistSq = std::max(maxDistSq, glm::length2(supportP));

            //// Check termination condition
            float depth = glm::dot(nearest->normal, supportP);
            float diff = depth - nearest->distance;
            if (depth < 0 ||
                std::abs(diff) < RELATIVE_TOLERANCE * glm::sqrt(maxDistSq) ||
                iteration++ > 20)
            {
//                std::cout << " *** EPA *** " << glm::to_string(nearest->normal) << " " << nearest->distance << "\n";
                return DistanceResult{nearest->normal, -nearest->distance};
            }

            //// Expand polytope
            // Remove visible faces from the new support point, extracting the boundary edges.
            std::unordered_set<std::pair<uint32_t, uint32_t>> boundaryEdges;
            for (int i = polytope.size() - 1; i >= 0; --i)
            {
                if (glm::dot(polytope[i].normal, supportP) > polytope[i].distance)
                    // 서포트 포인트에서 폴리토프를 봤을 때 면의 법선이 양수인 면들은 삭제
                    polytope.erase(polytope.begin() + i);
                else
                {
                    for (const auto& edge : polytope[i].edges)
                    {
                        if (boundaryEdges.contains(edge))
                            boundaryEdges.erase(edge);
                        else if (auto it = boundaryEdges.find({edge.second, edge.first}); it != boundaryEdges.end())
                            boundaryEdges.erase(it);
                        else
                            boundaryEdges.insert(edge);
                    }
                }
            }

            // Construct new faces from the boundary edges and the new support point.
            const int lastIdx = points.size() - 1;
            for (const auto& edge : boundaryEdges)
            {
                if (auto normal= GetNormal(supportP, points[edge.second], points[edge.first]))
                {
                    polytope.emplace_back(lastIdx, edge.second, edge.first,
                        normal.value(), glm::dot(normal.value(), points[lastIdx]));
                }
                else
                {
                    TMT_WARN << "Degenerate polytope plane";
                    return std::nullopt;
                }
            }
        }
    }
}