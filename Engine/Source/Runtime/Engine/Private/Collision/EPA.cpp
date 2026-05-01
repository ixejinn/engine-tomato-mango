#include <list>
#include <limits>
#include <glm/glm.hpp>
#include "Collision/EPA.h"
#include "Collision/GJK.h"
#include "Collision/CollisionEvent.h"
#include "Event/EventDispatcher.h"
#include "Math/Normal.h"
#include "Utils/Logger.h"

namespace tomato {
    EPAPlain::EPAPlain(
            std::vector<glm::vec3> &simplex,
            size_t idx0, size_t idx1, size_t idx2) {
        edges[0] = UnorderedPair<size_t>(idx0, idx1);
        edges[1] = UnorderedPair<size_t>(idx0, idx2);
        edges[2] = UnorderedPair<size_t>(idx1, idx2);

        normal = -GetOrientedNormal(glm::vec3(0.f), simplex[idx0], simplex[idx1], simplex[idx2]);

        distance = glm::dot(normal, simplex[idx0]);
    }

    void EPA::GetNormalDepth(const tomato::EPAEvent &e) {
        std::list<EPAPlain> plains;
        plains.emplace_back(e.simplex, 0, 1, 2);
        plains.emplace_back(e.simplex, 0, 1, 3);
        plains.emplace_back(e.simplex, 0, 2, 3);
        plains.emplace_back(e.simplex, 1, 2, 3);

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
            return;
        }

        e.simplex.push_back(
                GJK::Support(nearestPlain->normal, e.col1, e.trf1) -
                GJK::Support(-nearestPlain->normal, e.col2, e.trf2));
        auto dist = glm::dot(nearestPlain->normal, e.simplex.back());

        auto diff = dist - nearestPlain->distance;
        if (diff < 0)
            diff = -diff;

        if (diff < std::numeric_limits<float>::epsilon()) {
            EventDispatcher::GetInstance().Enqueue(CollisionEnterEvent())
            ////////////////////////////////////
        }
        else {
            // 확장
        }
    }
}