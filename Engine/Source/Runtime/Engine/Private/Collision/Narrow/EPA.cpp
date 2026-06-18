#include <limits>
#include <unordered_set>
#include <glm/glm.hpp>
#include "Collision/Narrow/EPA.h"
#include "Collision/Narrow/GJK.h"
#include "Collision/CollisionEvent.h"
#include "Event/EventDispatcher.h"
#include "Math/Normal.h"
#include "Utils/Logger.h"

namespace tomato {
    EPA::Plain::Plain(
        const std::vector<glm::vec3>& points,
        uint32_t idx0, uint32_t idx1, uint32_t idx2) {
        edges[0] = {idx0, idx1};
        edges[1] = {idx1, idx2};
        edges[2] = {idx2, idx0};

        normal = -GetOrientedNormal(glm::vec3{0.f}, points[idx0], points[idx1], points[idx2]);

        distance = glm::dot(normal, points[idx0]);
    }

    std::optional<CollisionResult> EPA::GetPenetrationInfo(
        std::vector<glm::vec3>& points, const ColliderComponent& col1, const ColliderComponent& col2, TransformComponent& trf1, TransformComponent& trf2) {
        if (points.size() != 4) {
            TMT_ERR << "Invalid simplex size: " << points.size();
            return std::nullopt;
        }

        std::vector<Plain> polytope;
        polytope.emplace_back(points, 0, 1, 2);
        polytope.emplace_back(points, 0, 1, 3);
        polytope.emplace_back(points, 0, 2, 3);
        polytope.emplace_back(points, 1, 2, 3);

        int iteration = 0;
        while (true) {
            Plain* nearest{nullptr};
            for (auto& plain : polytope) {
                if (!nearest || nearest->distance > plain.distance)
                    nearest = &plain;
            }

            if (!nearest) {
                TMT_ERR << "Incorrect polytope";
                return std::nullopt;
            }

            points.push_back(GJK::GetSupportPoint(nearest->normal, col1, trf1, col2, trf2));

            // Check termination condition
            float dist = glm::dot(nearest->normal, points.back());
            float diff = dist - nearest->distance;
            if (dist < 0 || (diff < 1e-3f && diff > -1e-3f) || ++iteration >= 32)
                return CollisionResult{nearest->normal, nearest->distance};

            // Expand polytope
            std::unordered_set<UnorderedPair<uint32_t>> edgesToExpand;
            for (int i = polytope.size() - 1; i >= 0; --i) {
                if (glm::dot(polytope[i].normal, points.back()) > polytope[i].distance)
                    // 서포트 포인트에서 폴리토프를 봤을 때 면의 법선이 양수인 면들은 삭제
                    polytope.erase(polytope.begin() + i);
                else {
                    for (const auto& edge : polytope[i].edges) {
                        if (edgesToExpand.contains(edge))
                            edgesToExpand.erase(edge);
                        else
                            edgesToExpand.insert(edge);
                    }
                }
            }

            const int lastIdx = points.size() - 1;
            for (const auto& edge : edgesToExpand)
                polytope.emplace_back(points, edge.a, edge.b, lastIdx);
        }
    }
}