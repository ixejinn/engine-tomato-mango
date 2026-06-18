#include "Collision/Narrow/GJK.h"
#include "ECS/Components/Collision.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Hierarchy.h"
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
//        {ColliderType::Capsule, support::Capsule}
    };

    std::optional<CollisionInfo> GJK::DetectCollision(
        entt::registry& reg, entt::entity e1, entt::entity e2) {
        auto& col1 = reg.get<ColliderComponent>(e1);
        auto& col2 = reg.get<ColliderComponent>(e2);

        if (col1.isTrigger || col2.isTrigger) {
            if (GJKBool(reg, e1, e2))
                return CollisionInfo{};
            else
                return std::nullopt;
        }
        else
            return GJKRaycast(reg, e1, e2);
    }

    glm::vec3 GJK::GetSupportPoint(
                const glm::vec3& worldDir,
                const ColliderComponent& col1, TransformComponent& trf1,
                const ColliderComponent& col2, TransformComponent& trf2) {
        return Support(worldDir, col1, trf1) - Support(-worldDir, col2, trf2);
    }

    bool GJK::GJKBool(
            entt::registry& reg, entt::entity e1, entt::entity e2) {
        auto& col1 = reg.get<ColliderComponent>(e1);
        auto& col2 = reg.get<ColliderComponent>(e2);
        auto& trf1 = reg.get<TransformComponent>(e1);
        auto& trf2 = reg.get<TransformComponent>(e2);

        std::vector<glm::vec3> simplex;
        simplex.reserve(4);

        glm::vec3 wPosCol1 = trf1.GetWorldPosition();
        glm::vec3 wPosCol2 = trf2.GetWorldPosition();
        glm::vec3 closestP = GetSupportPoint(wPosCol1 - wPosCol2, col1, trf1, col2, trf2);
        glm::vec3 supportP = GetSupportPoint(-closestP, col1, trf1, col2, trf2);
        simplex.push_back(supportP);

        int iteration = 0;
        while (++iteration < 20) {
            if (auto result = FindClosestPointOnSimplex(simplex))
                closestP = *result;
            else
                return true;

            supportP = GetSupportPoint(-closestP, col1, trf1, col2, trf2);
            simplex.push_back(supportP);
            if (glm::dot(-closestP, supportP) < 1e-6f)
                return false;   // 거의 가깝게 가는데 원점을 포함은 못하는 상황
        }
        return true;
    }

    std::optional<CollisionInfo> GJK::GJKDistance(
            entt::registry& reg, entt::entity e1, entt::entity e2) {
        TMT_INFO << "========== GJK distance " << (int)e1 << " " << (int)e2;
        auto& col1 = reg.get<ColliderComponent>(e1);
        auto& col2 = reg.get<ColliderComponent>(e2);
        auto& trf1 = reg.get<TransformComponent>(e1);
        auto& trf2 = reg.get<TransformComponent>(e2);

        glm::vec3 wPosCol1 = trf1.GetWorldPosition();
        glm::vec3 wPosCol2 = trf2.GetWorldPosition();
        glm::vec3 closestP = GetSupportPoint(wPosCol1 - wPosCol2, col1, trf1, col2, trf2);
        glm::vec3 supportP = GetSupportPoint(-closestP, col1, trf1, col2, trf2);

        std::vector<glm::vec3> simplex;
        simplex.reserve(4);

        auto upperLimit = glm::length2(closestP);
        auto lowerLimit = glm::dot(closestP, supportP);
        // while (glm::length2(closestP) - glm::dot(closestP, supportP) > 1e-6f) {
        while (upperLimit - lowerLimit > 1e-6f) {
            simplex.push_back(supportP);

            if (auto result = FindClosestPointOnSimplex(simplex))
                closestP = *result;
            else {

                return EPA::GetPenetrationInfo(simplex, col1, col2, trf1, trf2);
                // return CollisionInfo{glm::vec3{0.f}, 0.f};
            }

            supportP = GetSupportPoint(-closestP, col1, trf1, col2, trf2);

            upperLimit = glm::length2(closestP);
            lowerLimit = glm::dot(closestP, supportP);
        }

        auto length = glm::length(closestP);
        if (length > 1e-4f)
            return CollisionInfo{closestP, length};
        return EPA::GetPenetrationInfo(simplex, col1, col2, trf1, trf2);
        // return CollisionInfo{glm::vec3{0.f}, 0.f};
    }

    std::optional<CollisionInfo> GJK::GJKRaycast(
            entt::registry& reg, entt::entity e1, entt::entity e2) {
        auto& col1 = reg.get<ColliderComponent>(e1);
        auto& col2 = reg.get<ColliderComponent>(e2);
        auto& trf1 = reg.get<TransformComponent>(e1);
        auto& trf2 = reg.get<TransformComponent>(e2);

        glm::vec3 v1{0.f};
        glm::vec3 v2{0.f};

        if (auto vel1 = reg.try_get<VelocityComponent>(GetRootEntity(reg, e1)))
            v1 = vel1->velocity;
        if (auto vel2 = reg.try_get<VelocityComponent>(GetRootEntity(reg, e2)))
            v2 = vel2->velocity;

        float lenV1 = glm::length(v1);
        float lenV2 = glm::length(v2);
        float sumV = lenV1 + lenV2;
        if (sumV < 1e-6f)
            return std::nullopt;
        float weight = lenV1 / sumV;

        glm::vec3 relVel = v1 - v2;
        if (glm::length2(relVel) < 1e-6f) {
//            return GJKDistance(reg, e1, e2);
            return std::nullopt;
        }
        glm::vec3 ray = -relVel * FIXED_DELTA_TIME;
        // TMT_INFO << "GJK raycast " << (int)e1 << " " << (int)e2 << " relVel: " << relVel.x << " " << relVel.y << " " << relVel.z;

        float hitFraction = 0.f;
        glm::vec3 rayOrigin{0.f};
        glm::vec3 curRayPos = rayOrigin;
        glm::vec3 searchDir = curRayPos - GetSupportPoint(ray, col1, trf1, col2, trf2);  // CSO → curRayPos
        // glm::vec3 hitNormal{0.f};
        glm::vec3 hitNormal = searchDir;
        std::vector<glm::vec3> simplex;

        float maxDistSq = 1.f;
        int iteration = 0;
        while (glm::length2(searchDir) > 1e-6f * maxDistSq && iteration < 20) {
            // TMT_INFO << "(" << iteration << ") maxDistSq: " << maxDistSq;
            ++iteration;

            glm::vec3 supportP = GetSupportPoint(searchDir, col1, trf1, col2, trf2);
            glm::vec3 supportToRay = curRayPos - supportP;                         // 새로 얻은 심플렉스 점 → curRayPos

            float dotVW = glm::dot(searchDir, supportToRay);
            // TMT_INFO << "(" << iteration << ") dotVW: " << dotVW;
            if (dotVW > 0) {
                // 새로 얻은 심플렉스 점이 아직 curRayPos에 미치지 못함
                // curRayPos가 아직 CSO 외부에 있으므로 ray 전진 가능

                float dotVR = glm::dot(searchDir, ray);
                // TMT_INFO << "(" << iteration << ") dotVR: " << dotVR;
                if (dotVR >= -1e-5f) {
                    TMT_INFO << "Ray가 닿을 수 없음 " << dotVR << " | relative velocity: " << relVel.x << " " << relVel.y << " " << relVel.z;
                    // Ray와 CSO가 같은 방향(평행) 또는 수직으로 멀어짐
                    // Ray를 계속 전진시켜도 CSO에 닿을 수 없음
                    return std::nullopt;
                }

                // Ray가 CSO를 향하므로 ray를 전진
                hitFraction -= dotVW / dotVR;
                if (hitFraction > 1) {
                    TMT_INFO << "비충돌 종료";
                    // 이번 틱에서 충돌하지 않음 (비충돌 종료)
                    return std::nullopt;
                }

                glm::vec3 preRayPos = curRayPos;
                curRayPos = rayOrigin + hitFraction * ray;

                glm::vec3 deltaRay = curRayPos - preRayPos;
                for (auto& p : simplex)
                    p += deltaRay;

                hitNormal = searchDir;
            }

            simplex.push_back(curRayPos - supportP);    // Support point
            if (auto result = FindClosestPointOnSimplex(simplex))
                searchDir = result.value();
            else {
                if (!col1.isTrigger && !col2.isTrigger) {
                    TMT_WARN << "Simplex already encloses origin. " << (int)e1 << " " << (int)e2;

                    if (auto info = EPA::GetPenetrationInfo(simplex, col1, col2, trf1, trf2)) {
                        TMT_INFO << "Penetration normal: " << info->normal.x << " " << info->normal.y << " " << info->normal.z;
                        TMT_INFO << "Penetration depth : " << info->depth;
                    }
                }
                break;
            }

            maxDistSq = 1e-6f;
            for (auto& p : simplex)
                maxDistSq = std::max(maxDistSq, glm::length2(curRayPos - p));

            // TMT_INFO << "(" << iteration << ") searchDir: " << searchDir.x << " " << searchDir.y << " " << searchDir.z;
        }

        if (hitFraction <= 1) {
            float hitNormalLenSq = glm::length2(hitNormal);
            if (hitNormalLenSq > 1e-6f) {
                constexpr float epsilon = 0.001f;
                if (-epsilon < hitNormal.x && hitNormal.x < epsilon)
                    hitNormal.x = 0.f;
                if (-epsilon < hitNormal.z && hitNormal.z < epsilon)
                    hitNormal.z = 0.f;
                if (-epsilon < hitNormal.y && hitNormal.y < epsilon)
                    hitNormal.y = 0.f;

                hitNormal = glm::normalize(hitNormal);
            }

            return CollisionInfo{hitNormal, hitFraction, weight};
        }

        TMT_INFO << "기타 비충돌 종료";
        return std::nullopt;
    }

    glm::vec3 GJK::Support(
            const glm::vec3& worldDir,
            const ColliderComponent& col, TransformComponent& trf) {
        const auto worldRot = glm::toMat4(trf.GetWorldQuaternion());

        const glm::vec3 localDir = glm::transpose(worldRot) * glm::vec4(worldDir, 0.f);

        const auto localSupportP = supportFunctions_[col.type](localDir, trf);
        return trf.GetWorldPosition() + glm::vec3{worldRot * glm::vec4(localSupportP, 1.f)};
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
                TMT_ERR << "Incorrect simplex size: " << simplex.size();
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
}