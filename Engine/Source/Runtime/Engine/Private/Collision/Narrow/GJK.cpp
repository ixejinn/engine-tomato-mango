#include "Collision/Narrow/GJK.h"
#include "ECS/Components/Collision.h"
#include "ECS/Components/Transform.h"
#include "Collision/ColliderSupport.h"
#include "Collision/CollisionEvent.h"
#include "Collision/Narrow/EPA.h"
#include "ECS/Components/Rigidbody.h"
#include "Math/Normal.h"
#include "Utils/Logger.h"
#include "SimulationConfig.h"

namespace tomato {
    // Registry support function per collider
    EnumArray<ColliderType, GJK::SupportFunc> GJK::supportFunctions_ = {
        {ColliderType::Cube, support::Cube},
        {ColliderType::Sphere, support::Sphere},
        {ColliderType::Capsule, support::Capsule}
    };

    std::optional<CollisionInfo> GJK::DetectCollision(
        entt::registry& reg, entt::entity e1, entt::entity e2) {
        auto& col1 = reg.get<ColliderComponent>(e1);
        auto& col2 = reg.get<ColliderComponent>(e2);
        auto& trf1 = reg.get<TransformComponent>(e1);
        auto& trf2 = reg.get<TransformComponent>(e2);

        auto vel1 = reg.try_get<VelocityComponent>(e1);
        auto vel2 = reg.try_get<VelocityComponent>(e2);

        glm::vec3 relVel = (vel1 ? vel1->velocity : glm::vec3{0.f}) - (vel2 ? vel2->velocity : glm::vec3{0.f}) * FIXED_DELTA_TIME;
        glm::vec3 rayDir = -relVel;

        float hitFraction = 0.f;
        glm::vec3 rayOrigin{0.f};
        glm::vec3 curRayPos = rayOrigin;
        glm::vec3 hitNormal{0.f};
        glm::vec3 searchDir = curRayPos - GetSupportPoint(rayDir, col1, trf1, col2, trf2);  // CSO → curRayPos
        std::vector<glm::vec3> simplex;

        while (glm::length2(searchDir) > 1e-6f) {
            glm::vec3 supportP = GetSupportPoint(searchDir, col1, trf1, col2, trf2);
            glm::vec3 supportToRay = curRayPos - supportP;                          // 새로 얻은 심플렉스 점 → curRayPos

            float dotVW = glm::dot(searchDir, supportToRay);
            if (dotVW > 0) {
                // 새로 얻은 심플렉스 점이 아직 curRayPos에 미치지 못함
                // curRayPos가 아직 CSO 외부에 있으므로 ray 전진 가능

                float dotVR = glm::dot(searchDir, rayDir);
                if (dotVR >= -1e-5f)
                    // Ray와 CSO가 같은 방향(평행) 또는 수직으로 멀어짐
                    // Ray를 계속 전진시켜도 CSO에 닿을 수 없음
                    return std::nullopt;

                // Ray가 CSO를 향하므로 ray를 전진
                hitFraction -= dotVW / dotVR;
                if (hitFraction > 1)
                    // 이번 틱에서 충돌하지 않음 (비충돌 종료)
                    return std::nullopt;

                glm::vec3 preRayPos = curRayPos;
                curRayPos = rayOrigin + hitFraction * rayDir;

                glm::vec3 deltaPos = curRayPos - preRayPos;
                for (auto& p : simplex)
                    p += deltaPos;

                hitNormal = searchDir;
            }

            simplex.push_back(curRayPos - supportP);
            if (auto result = FindClosestPointOnSimplex(simplex))
                searchDir = *result;
            else {
                TMT_WARN << "Collision error";
                break;
            }
        }

        if (hitFraction <= 1) {
            float hitNormalLenSq = glm::length2(hitNormal);
            if (hitNormalLenSq > 1e-6f)
                return CollisionInfo{glm::normalize(hitNormal), hitFraction};
            else
                return CollisionInfo{hitNormal, 0.f};
        }

        return std::nullopt;
    }

    glm::vec3 GJK::GetSupportPoint(
                const glm::vec3& worldDir,
                const ColliderComponent& col1, TransformComponent& trf1,
                const ColliderComponent& col2, TransformComponent& trf2) {
        return Support(worldDir, col1, trf1) - Support(-worldDir, col2, trf2);
    }

    glm::vec3 GJK::Support(
            const glm::vec3& worldDir,
            const ColliderComponent& col, TransformComponent& trf) {
        const auto R = glm::toMat4(trf.GetQuaternion());
        const glm::vec4 localDir = glm::transpose(R) * glm::vec4(worldDir, 0.f);

        const auto localSupportP = supportFunctions_[col.type](localDir, col);
        return trf.GetPosition() + glm::vec3{R * glm::vec4(localSupportP, 1.f)};
    }

    std::optional<glm::vec3> GJK::FindClosestPointOnSimplex(std::vector<glm::vec3>& simplex) {
        switch (simplex.size()) {
            case 1:
                return simplex[0];

            case 2: {
                const auto ao = -simplex[0];
                const auto ab = simplex[1] - simplex[0];

                auto t = glm::dot(ao, ab);
                if (t <= 0) {
                    simplex.pop_back();
                    return simplex[0];
                }
                else {
                    float denom = glm::length2(ab);

                    if (t >= denom) {
                        simplex.erase(simplex.begin());
                        return simplex[0];
                    }
                    else {
                        t /= denom;
                        return simplex[0] + t * ab;
                    }
                }
            }

            case 3:
                return FindClosestPointOnTriangle(simplex);

            case 4: {
                glm::vec3 p{0.f};
                glm::vec3 a = simplex[0];
                glm::vec3 b = simplex[1];
                glm::vec3 c = simplex[2];
                glm::vec3 d = simplex[3];

                glm::vec3 closestP = p;
                float bestSqDist = std::numeric_limits<float>::max();
                std::vector<glm::vec3> bestSimplex;

                // ABC
                if (PointOutsideOfPlane(p, a, b, c, d)) {
                    auto q = ClosestPtPointTriangle(p, a, b, c);
                    float sqDist = glm::length2(q - p);

                    if (sqDist < bestSqDist) {
                        bestSqDist = sqDist;
                        closestP = q;
                        bestSimplex = {a, b, c};
                    }
                }

                // ACD
                if (PointOutsideOfPlane(p, a, c, d, b)) {
                    auto q = ClosestPtPointTriangle(p, a, c, d);
                    float sqDist = glm::length2(q - p);
                    if (sqDist < bestSqDist) {
                        bestSqDist = sqDist;
                        closestP = q;
                        bestSimplex = {a, c, d};
                    }
                }

                // ADB
                if (PointOutsideOfPlane(p, a, d, b, c)) {
                    auto q = ClosestPtPointTriangle(p, a, d, b);
                    float sqDist = glm::length2(q - p);
                    if (sqDist < bestSqDist) {
                        bestSqDist = sqDist;
                        closestP = q;
                        bestSimplex = {a, d, b};
                    }
                }

                // BDC
                if (PointOutsideOfPlane(p, b, d, c, a)) {
                    auto q = ClosestPtPointTriangle(p, b, d, c);
                    float sqDist = glm::length2(q - p);
                    if (sqDist < bestSqDist) {
                        bestSqDist = sqDist;
                        closestP = q;
                        bestSimplex = {b, d, c};
                    }
                }

                if (bestSimplex.empty())
                    return std::nullopt;
                else {
                    simplex = bestSimplex;
                    return closestP;
                }
            }

            default:
                TMT_ERR << "Incorrect simplex size";
                return glm::vec3{0.f};
        }
    }

    glm::vec3 GJK::FindClosestPointOnTriangle(std::vector<glm::vec3>& simplex) {
        const glm::vec3 a = simplex[0];
        const glm::vec3 b = simplex[1];
        const glm::vec3 c = simplex[2];

        // Vertex A region
        const glm::vec3 ab = b - a;
        const glm::vec3 ac = c - a;
        const glm::vec3 ao = -a;

        const float d1 = glm::dot(ab, ao);
        const float d2 = glm::dot(ac, ao);
        if (d1 <= 0.f && d2 <= 0.f) {
            simplex = {a};
            return a;
        }

        // Vertex B region
        glm::vec3 bo = -b;

        const float d3 = glm::dot(ab, bo);
        const float d4 = glm::dot(ac, bo);
        if (d3 >= 0.f && d4 <= d3) {
            simplex = {b};
            return b;
        }

        // Edge AB region
        const float vc = d1 * d4 - d3 * d2;
        if (vc <= 0.f && d1 >= 0.f && d3 <= 0.f) {
            float v = d1 / (d1 - d3);
            simplex = {a, b};
            return a + v * ab;
        }

        // Vertex C region
        const glm::vec3 co = -c;

        const float d5 = glm::dot(ab, co);
        const float d6 = glm::dot(ac, co);
        if (d6 >= 0.f && d5 <= d6) {
            simplex = {c};
            return c;
        }

        // Edge AC region
        const float vb = d5 * d2 - d1 * d6;
        if (vb <= 0.f && d2 >= 0.f && d6 <= 0.f) {
            float w = d2 / (d2 - d6);
            simplex = {a, c};
            return a + w * ac;
        }

        // Edge BC region
        const float va = d3 * d6 - d5 * d4;
        if (va <= 0.f && (d4 - d3) >= 0.f && (d5 - d6) >= 0.f) {
            float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
            simplex = {b, c};
            return b + w * (c - b);
        }

        // Face region
        float denom = 1.f / (va + vb + vc);
        float v = vb * denom;
        float w = vc * denom;
        return a + ab * v + ac * w;
    }

    glm::vec3 GJK::ClosestPtPointTriangle(
            const glm::vec3& p,
            const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
        // Vertex A region
        const glm::vec3 ab = b - a;
        const glm::vec3 ac = c - a;
        const glm::vec3 ap = p - a;

        const float d1 = glm::dot(ab, ap);
        const float d2 = glm::dot(ac, ap);
        if (d1 <= 0.f && d2 <= 0.f) {
            return a;
        }

        // Vertex B region
        glm::vec3 bp = p - b;

        const float d3 = glm::dot(ab, bp);
        const float d4 = glm::dot(ac, bp);
        if (d3 >= 0.f && d4 <= d3) {
            return b;
        }

        // Edge AB region
        const float vc = d1 * d4 - d3 * d2;
        if (vc <= 0.f && d1 >= 0.f && d3 <= 0.f) {
            float v = d1 / (d1 - d3);
            return a + v * ab;
        }

        // Vertex C region
        const glm::vec3 cp = p - c;

        const float d5 = glm::dot(ab, cp);
        const float d6 = glm::dot(ac, cp);
        if (d6 >= 0.f && d5 <= d6) {
            return c;
        }

        // Edge AC region
        const float vb = d5 * d2 - d1 * d6;
        if (vb <= 0.f && d2 >= 0.f && d6 <= 0.f) {
            float w = d2 / (d2 - d6);
            return a + w * ac;
        }

        // Edge BC region
        const float va = d3 * d6 - d5 * d4;
        if (va <= 0.f && (d4 - d3) >= 0.f && (d5 - d6) >= 0.f) {
            float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
            return b + w * (c - b);
        }

        // Face region
        float denom = 1.f / (va + vb + vc);
        float v = vb * denom;
        float w = vc * denom;
        return a + ab * v + ac * w;
    }

    int GJK::PointOutsideOfPlane(
            const glm::vec3& p,
            const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
        return glm::dot(p - a, glm::cross(b - a, c - a)) >= 0.f;
    }

    int GJK::PointOutsideOfPlane(
            const glm::vec3& p,
            const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d) {
        float signp = glm::dot(p - a, glm::cross(b - a, c - a));
        float signd = glm::dot(d - a, glm::cross(b - a, c - a));
        return signp * signd < 0.f;
    }

    bool GJK::AddSimplexPoint(
            std::vector<glm::vec3>& simplex,
            const ColliderComponent& col1, TransformComponent& trf1,
            const ColliderComponent& col2, TransformComponent& trf2) {
        glm::vec3 dir;
        auto simplexSize = simplex.size();

        switch (simplexSize)
        {
            case 0:
            {
                glm::vec3 wPosCol1 = trf1.GetTransformMatrix() * glm::vec4(col1.position, 1.f);
                glm::vec3 wPosCol2 = trf2.GetTransformMatrix() * glm::vec4(col2.position, 1.f);

                simplex.push_back(GetSupportPoint(wPosCol2 - wPosCol1, col1, trf1, col2, trf2));
            }
                break;

            case 1:
                dir = -simplex[0];
                simplex.push_back(GetSupportPoint(dir, col1, trf1, col2, trf2));

                if (glm::dot(dir, simplex[1]) < 0)
                    return false;   // 심플렉스가 원점을 포함할 수 없음
                break;

            case 2:
            {
                const auto ab = simplex[1] - simplex[0];
                const auto ao = -simplex[0];
                dir = glm::cross(glm::cross(ab, ao), ab);
                simplex.push_back(GetSupportPoint(dir, col1, trf1, col2, trf2));

                if (glm::dot(dir, simplex[2]) < 0)
                    return false;
            }
                break;

            case 3:
                dir = GetOrientedNormal(glm::vec3{0.f}, simplex[0], simplex[1], simplex[2]);
                simplex.push_back(GetSupportPoint(dir, col1, trf1, col2, trf2));

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