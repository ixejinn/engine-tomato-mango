#include <list>
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
    EPAPlain::EPAPlain(
            const std::vector<glm::vec3>& simplex,
            uint32_t idx0, uint32_t idx1, uint32_t idx2) {
        SetPlain(simplex, idx0, idx1, idx2);
    }

    EPAPlain::EPAPlain(
        const std::vector<glm::vec3>& simplex,
        uint32_t idx0, uint32_t idx1) {
        SetPlain(simplex, idx0, idx1, simplex.size() - 1);
    }

    void EPAPlain::SetPlain(
        const std::vector<glm::vec3>& simplex,
        uint32_t idx0, uint32_t idx1, uint32_t idx2) {
        edgeIndices[0] = UnorderedPair<uint32_t>(idx0, idx1);
        edgeIndices[1] = UnorderedPair<uint32_t>(idx0, idx2);
        edgeIndices[2] = UnorderedPair<uint32_t>(idx1, idx2);

        normal = -GetOrientedNormal(glm::vec3(0.f), simplex[idx0], simplex[idx1], simplex[idx2]);

        distance = glm::dot(normal, simplex[idx0]);
    }

    std::optional<CollisionInfo> EPA::GetNormalDepth(std::vector<glm::vec3>& simplex,
                 const ColliderComponent& col1, const ColliderComponent& col2,
                 const TransformComponent& trf1, const TransformComponent& trf2) {
        std::list<EPAPlain> plains;
        plains.emplace_back(simplex, 0, 1, 2);
        plains.emplace_back(simplex, 0, 1, 3);
        plains.emplace_back(simplex, 0, 2, 3);
        plains.emplace_back(simplex, 1, 2, 3);

        while (true) {
            float minDist = std::numeric_limits<float>::max();
            EPAPlain* nearestPlain = nullptr;
            for (auto& plain : plains) {
                if (minDist > plain.distance) {
                    minDist = plain.distance;
                    nearestPlain = &plain;
                }
            }
            if (!nearestPlain)
            {
                TMT_ERR << "Incorrect nearest plain.";
                return std::nullopt;
            }

            simplex.push_back(
                    GJK::Support(nearestPlain->normal, col1, trf1) -
                    GJK::Support(-nearestPlain->normal, col2, trf2));
            auto dist = glm::dot(nearestPlain->normal, simplex.back());

            auto diff = dist - nearestPlain->distance;
            if (diff < 0)
                diff = -diff;

            // 종료 조건
            if (diff < std::numeric_limits<float>::epsilon())
                return CollisionInfo{glm::normalize(nearestPlain->normal), nearestPlain->distance};

            // 확장
            std::unordered_set<UnorderedPair<uint32_t>> edges;
            for (auto it = plains.begin(); it != plains.end(); ) {
                if (glm::dot(it->normal, simplex.back()) >= 0)   // 앞 부분 면 삭제
                    it = plains.erase(it);
                else {
                    for (const auto& edgeIdx : it->edgeIndices) {
                        if (edges.contains(edgeIdx))
                            edges.erase(edgeIdx);
                        else
                            edges.insert(edgeIdx);
                    }

                    ++it;
                }
            }

            for (auto edge : edges)
                plains.emplace_back(simplex, edge.a, edge.b);
        }
    }
}