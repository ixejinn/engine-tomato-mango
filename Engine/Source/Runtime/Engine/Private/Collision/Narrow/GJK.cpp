#include "Collision/Narrow/GJK.h"
#include "ECS/Components/Collision.h"
#include "ECS/Components/Transform.h"
#include "Collision/ColliderSupport.h"
#include "Collision/CollisionEvent.h"
#include "Collision/Narrow/EPA.h"
#include "Math/Normal.h"
#include "Utils/Logger.h"

namespace tomato {
    // Registry support function per collider
    EnumArray<ColliderType, GJK::SupportFunc> GJK::supportFunctions_ = {
        {ColliderType::Cube, support::Cube},
        {ColliderType::Sphere, support::Sphere},
        {ColliderType::Capsule, support::Capsule}
    };

    std::optional<CollisionInfo> GJK::DetectCollision(
        const ColliderComponent &col1, const TransformComponent &trf1,
        const ColliderComponent &col2, const TransformComponent &trf2) {
        glm::vec3 wPosCol1 = trf1.GetTransformMatrix() * glm::vec4(col1.position, 1.f);
        glm::vec3 wPosCol2 = trf2.GetTransformMatrix() * glm::vec4(col2.position, 1.f);
        glm::vec3 closestP = GetSupportPoint(wPosCol1 - wPosCol2, col1, trf1, col2, trf2);
        glm::vec3 supportP = GetSupportPoint(-closestP, col1, trf1, col2, trf2);

        std::vector<glm::vec3> simplex;
        simplex.reserve(4);

        static constexpr auto epsilonSqr = std::numeric_limits<float>::epsilon() * std::numeric_limits<float>::epsilon();
        while (glm::length2(closestP) - glm::dot(closestP, supportP) > epsilonSqr) {
            simplex.push_back(supportP);

            if (auto result = FindClosestPointOnSimplex(simplex))
                closestP = *result;
            else
                return EPA::GetNormalDepth(simplex, col1, col2, trf1, trf2);

            supportP = GetSupportPoint(-closestP, col1, trf1, col2, trf2);
        }

        auto length = glm::length(closestP);
        if (length > 1.f)
            return std::nullopt;
        return CollisionInfo{glm::vec3{0.f}, -length};

        while (true)
        {
            if (!AddSimplexPoint(simplex, col1, trf1, col2, trf2))
                // 비충돌 종료
                return std::nullopt;

            if (VoronoiRegion(simplex)) {
                // 충돌 종료
                if (col1.isTrigger || col2.isTrigger)
                    return CollisionInfo{};
                return EPA::GetNormalDepth(simplex, col1, col2, trf1, trf2);
            }
        }
    }

    glm::vec3 GJK::GetSupportPoint(
                const glm::vec3& worldDir,
                const ColliderComponent& col1, const TransformComponent& trf1,
                const ColliderComponent& col2, const TransformComponent& trf2) {
        return Support(worldDir, col1, trf1) - Support(-worldDir, col2, trf2);
    }

    glm::vec3 GJK::Support(
            const glm::vec3& worldDir,
            const ColliderComponent& col, const TransformComponent& trf) {
        const auto R = glm::toMat4(trf.GetQuaternion());
        const glm::vec4 localDir = glm::transpose(R) * glm::vec4(worldDir, 0.f);

        const auto localSupportP = supportFunctions_[col.type](localDir, col);
        return trf.GetPosition() + glm::vec3{R * glm::vec4(col.position + localSupportP, 1.f)};
    }

    std::optional<glm::vec3> GJK::FindClosestPointOnSimplex(std::vector<glm::vec3>& simplex) {
        switch (simplex.size()) {
            case 1:
                return simplex[0];

            case 2: {
                const auto ao = -simplex[0];
                const auto ab = simplex[1] - simplex[0];

                const auto x = glm::dot(ao, ab);
                if (x <= 0) {
                    simplex.pop_back();
                    return simplex[0];
                }
                else if (x >= glm::length2(ab)) {
                    simplex.erase(simplex.begin());
                    return simplex[0];
                }

                return simplex[0] + ab * glm::dot(ab, ao) / glm::length2(ab);
            }

            case 3:
                return FindClosestPointOnTriangle(simplex);

            case 4: {
                glm::vec3 closestP{0.f};

                const auto lo = -simplex[3];
                if (glm::dot(-GetOrientedNormal(simplex[1], simplex[3], simplex[0], simplex[2]), lo) >= 0) {
                    simplex.erase(++simplex.begin());
                    return FindClosestPointOnTriangle(simplex);
                }
                if (glm::dot(-GetOrientedNormal(simplex[0], simplex[3], simplex[2], simplex[1]), lo) >= 0) {
                    simplex.erase(simplex.begin());
                    return FindClosestPointOnTriangle(simplex);
                }
                if (glm::dot(-GetOrientedNormal(simplex[2], simplex[3], simplex[1], simplex[0]), lo) >= 0) {
                    simplex.erase(simplex.begin() + 2);
                    return FindClosestPointOnTriangle(simplex);
                }

                return std::nullopt;
            }

            default:
                TMT_ERR << "Incorrect simplex size";
                return glm::vec3{0.f};
        }
    }

    // 가장 나중에 추가된 점이 포함되는 feature에 대해서만 확인
    glm::vec3 GJK::FindClosestPointOnTriangle(std::vector<glm::vec3>& simplex) {
        const glm::vec3 ca = simplex[0] - simplex[2];
        const glm::vec3 cb = simplex[1] - simplex[2];
        const glm::vec3 co = -simplex[2];

        const auto dotCA = glm::dot(co, ca);
        const auto dotCB = glm::dot(co, cb);

        const auto len2CA = glm::length2(ca);
        const auto len2CB = glm::length2(cb);

        if (dotCA <= 0 && dotCB <= 0) {
            simplex[0] = simplex[2];
            simplex.pop_back();
            simplex.pop_back();
            return simplex[0];
        }

        const auto normal = GetOrientedNormal(glm::vec3{0.f}, simplex[0], simplex[1], simplex[2]);

        if (dotCA >= 0 && dotCB < 0 && dotCA <=len2CA) {
            simplex.erase(++simplex.begin());
            return simplex[1] + ca * dotCA / len2CA;
        }

        if (dotCB >= 0 && dotCA < 0 && dotCB <=len2CB) {
            simplex.erase(simplex.begin());
            return simplex[1] + cb * dotCB / len2CB;
        }

        return glm::dot(normal, -simplex[0]) / glm::length2(normal) * -normal;
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
}