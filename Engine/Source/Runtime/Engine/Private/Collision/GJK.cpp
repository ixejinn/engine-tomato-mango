#include "Collision/GJK.h"
#include "ECS/Components/Collision.h"
#include "ECS/Components/Transform.h"
#include "Collision/ColliderSupport.h"
#include "Utils/Logger.h"

namespace tomato {
    // Registry support function per collider
    EnumArray<ColliderType, GJK::SupportFunc> GJK::supportFunctions_ = {
        {ColliderType::Cube, support::Cube},
        {ColliderType::Sphere, support::Sphere},
        {ColliderType::Capsule, support::Capsule}
    };

    bool GJK::CheckCollision(const tomato::ColliderComponent &col1, const tomato::TransformComponent &trf1,
                             const tomato::ColliderComponent &col2, const tomato::TransformComponent &trf2) {
        std::vector<glm::vec3> simplex;
        simplex.reserve(4);

        while (true)
        {
            if (!AddSimplexPoint(simplex, col1, trf1, col2, trf2))
                return false;   // 비충돌 종료

            if (VoronoiRegion(simplex))
                return true;    // 충돌 종료
        }
    }

    glm::vec3 GJK::Support(
            const glm::vec3& worldDir,
            const ColliderComponent& col, const TransformComponent& trf) {
        const auto R = glm::toMat4(trf.GetQuaternion());
        const glm::vec4 localDir = glm::transpose(R) * glm::vec4(worldDir, 0.f);

        const auto localSupportP = supportFunctions_[col.type](localDir, col);
        return trf.GetPosition() + glm::vec3{R * glm::vec4(col.position + localSupportP, 1.f)};
    }

    bool GJK::AddSimplexPoint(
            std::vector<glm::vec3>& simplex,
            const ColliderComponent& col1, const TransformComponent& trf1,
            const ColliderComponent& col2, const TransformComponent& trf2) {
        glm::vec3 dir;
        auto simplexSize = simplex.size();

        switch (simplexSize)
        {
            case 0:
            {
                glm::vec3 wPosCol1 = trf1.GetTransformMatrix() * glm::vec4(col1.position, 1.f);
                glm::vec3 wPosCol2 = trf2.GetTransformMatrix() * glm::vec4(col2.position, 1.f);

                simplex.push_back(Support(wPosCol2 - wPosCol1, col1, trf1) - Support(wPosCol1 - wPosCol2, col2, trf2));
            }
                break;

            case 1:
                dir = -simplex[0];
                simplex.push_back(Support(dir, col1, trf1) - Support(-dir, col2, trf2));

                if (glm::dot(dir, simplex[1]) < 0)
                    return false;   // 심플렉스가 원점을 포함할 수 없음
                break;

            case 2:
            {
                const auto ab = simplex[1] - simplex[0];
                const auto ao = -simplex[0];
                dir = glm::cross(glm::cross(ab, ao), ab);
                simplex.push_back(Support(dir, col1, trf1) - Support(-dir, col2, trf2));

                if (glm::dot(dir, simplex[2]) < 0)
                    return false;
            }
                break;

            case 3:
                dir = GetOrientedNormal(glm::vec3{0.f}, simplex[0], simplex[1], simplex[2]);
                simplex.push_back(Support(dir, col1, trf1) - Support(-dir, col2, trf2));

                if (glm::dot(dir, simplex[3]) < 0)
                    return false;
                break;

            default:
                TMT_WARN << "Incorrect simplex points.";
        }

        return true;
    }

    bool GJK::VoronoiRegion(std::vector<glm::vec3> &simplex) {
        auto simplexSize = simplex.size();
        switch (simplexSize)
        {
            case 1:
                // if (simplex[0] == Vector3{0.f}) return true;
                return false;

            case 2:
            {
                const auto ao = -simplex[0];
                const auto ab = simplex[1] - simplex[0];

                const auto dotV = glm::dot(ao, ab);
                if (dotV < 0)
                    simplex.pop_back();
                else if (dotV > glm::dot(ab, ab))
                    simplex.erase(simplex.begin());

                return false;
            }

            case 3:
            {
                const auto ab = simplex[1] - simplex[0];
                const auto bc = simplex[2] - simplex[1];
                const auto ac = simplex[2] - simplex[0];
                const auto ao = -simplex[0];
                const auto bo = -simplex[1];

                const auto normal = glm::cross(ac, ab);

                if (glm::dot(ao, glm::cross(normal, ab)) > 0)
                {
                    simplex.pop_back();
                    return false;
                }

                if (glm::dot(bo, glm::cross(normal, bc)) > 0)
                {
                    simplex.erase(simplex.begin());
                    return false;
                }

                if (glm::dot(ao, glm::cross(normal, -ac)) > 0)
                {
                    simplex.erase(++simplex.begin());
                    return false;
                }

                // 2차원이면 return true;
                return false;
            }

            case 4:
            {
                const auto lo = -simplex[3];

                if (glm::dot(-GetOrientedNormal(simplex[1], simplex[3], simplex[0], simplex[2]), lo) > 0)
                {
                    simplex.erase(++simplex.begin());
                    return false;
                }
                if (glm::dot(-GetOrientedNormal(simplex[0], simplex[3], simplex[2], simplex[1]), lo) > 0)
                {
                    simplex.erase(simplex.begin());
                    return false;
                }
                if (glm::dot(-GetOrientedNormal(simplex[2], simplex[3], simplex[1], simplex[0]), lo) > 0)
                {
                    simplex.erase(simplex.begin() + 2);
                    return false;
                }
                return true;
            }

            default:
                TMT_WARN << "Incorrect simplex size.";
                return false;
        }
    }

    glm::vec3 GJK::GetOrientedNormal(
            const glm::vec3 &refP,
            const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2) {
        const auto vec01 = p1 - p0;
        const auto vec02 = p2 - p0;
        auto normal = glm::cross(vec01, vec02);
        auto x = glm::dot(normal, (refP - p0));
        return (x > 0 ? normal : -normal);
    }
}