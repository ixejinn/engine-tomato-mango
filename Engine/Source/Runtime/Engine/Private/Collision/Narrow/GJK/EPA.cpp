#include <unordered_set>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include "ECS/Components/Transform.h"
#include "ECS/Components/Collision.h"
#include "Collision/Narrow/GJK/EPA.h"
#include "Collision/Narrow/GJK/GJK.h"
#include "Collision/CollisionConfig.h"
#include "Math/Normal.h"
#include "Utils/Logger.h"

namespace tomato
{
    EPAPlain::EPAPlain(
        const std::vector<glm::vec3>& points,
        uint32_t idx0, uint32_t idx1, uint32_t idx2)
    {
        edges[0] = {idx0, idx1};
        edges[1] = {idx1, idx2};
        edges[2] = {idx2, idx0};

        normal = -GetOrientedNormal(glm::vec3{0.f}, points[idx0], points[idx1], points[idx2]);

        distance = glm::dot(normal, points[idx0]);
    }

    EPAPlain::EPAPlain(
        const std::vector<glm::vec3>& points,
        uint32_t ref,
        uint32_t idx0, uint32_t idx1, uint32_t idx2)
    {
        edges[0] = { idx0, idx1 };
        edges[1] = { idx1, idx2 };
        edges[2] = { idx2, idx0 };

        normal = -GetOrientedNormal(points[ref], points[idx0], points[idx1], points[idx2]);

        distance = glm::dot(normal, points[idx0]);
    }

    std::optional<DistanceResult> RunEPA(
    std::vector<glm::vec3>& points,
        const ColliderComponent& col1, TransformComponent& trf1,
        const ColliderComponent& col2, TransformComponent& trf2)
    {
        std::cout << "========== EPA\n";

        // 심플렉스 확장
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

                points.emplace_back(GJK::GetSupportPoint(normal, col1, trf1, col2, trf2));
            }
            break;
            }
        }

        std::vector<EPAPlain> polytope;
        polytope.emplace_back(points, 3, 0, 1, 2);
        polytope.emplace_back(points, 2, 0, 1, 3);
        polytope.emplace_back(points, 1, 0, 2, 3);
        polytope.emplace_back(points, 0, 1, 2, 3);

        int iteration = 0;
        while (true)
        {
            EPAPlain* nearest{nullptr};
            for (auto& plain : polytope)
            {
                if (!nearest || nearest->distance > plain.distance)
                    nearest = &plain;
            }

            if (!nearest)
            {
                TMT_ERR << "Incorrect polytope";
                return std::nullopt;
            }

            //if (nearest->distance == 0)
            //{
            //    std::cout << " *** EPA(" << iteration << ")***" << glm::to_string(nearest->normal) << " " << nearest->distance << "\n";
            //    return DistanceResult{ nearest->normal, 0.f };
            //}

            points.push_back(GJK::GetSupportPoint(nearest->normal, col1, trf1, col2, trf2));

            // Check termination condition
            float dist = glm::dot(nearest->normal, points.back());
            float diff = dist - nearest->distance;
            if (dist < 0 ||
                (diff < EPSILON && diff > -EPSILON) ||
                iteration++ > 20)
            {
                std::cout << " *** EPA *** " << glm::to_string(nearest->normal) << " " << nearest->distance << "\n";
                return DistanceResult{ nearest->normal, -nearest->distance };
            }

            // Expand polytope
            std::unordered_set<UnorderedPair<uint32_t>> edgesToExpand;
            for (int i = polytope.size() - 1; i >= 0; --i)
            {
                if (glm::dot(polytope[i].normal, points.back()) > polytope[i].distance)
                    // 서포트 포인트에서 폴리토프를 봤을 때 면의 법선이 양수인 면들은 삭제
                    polytope.erase(polytope.begin() + i);
                else
                {
                    for (const auto& edge : polytope[i].edges)
                    {
                        if (edgesToExpand.contains(edge))
                            edgesToExpand.erase(edge);
                        else
                            edgesToExpand.insert(edge);
                    }
                }
            }

            const int lastIdx = points.size() - 1;
            for (const auto& edge : edgesToExpand)
            {
                int refIdx = 0;
                while (refIdx == edge.a || refIdx == edge.b || refIdx == lastIdx)
                    ++refIdx;

                polytope.emplace_back(points, refIdx, edge.a, edge.b, lastIdx);

                if (glm::length2(polytope.back().normal) == 0)
                    polytope.pop_back();
            }
        }
    }
}