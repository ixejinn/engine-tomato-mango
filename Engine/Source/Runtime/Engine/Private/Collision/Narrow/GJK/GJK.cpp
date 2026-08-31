#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include "Collision/Narrow/GJK/GJK.h"
#include "Collision/Narrow/GJK/EPA.h"
#include "Collision/ColliderSupport.h"
#include "Collision/CollisionEvent.h"
#include "Collision/CollisionConstants.h"
#include "Collision/CollisionConfig.h"
#include "ECS/Components/Collision.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Entity/Hierarchy.h"
#include "ECS/Components/Rigidbody.h"
#include "Simulation/SimulationConfig.h"
#include "Math/Normal.h"
#include "Utils/Logger.h"
#include "Event/EventDispatcher.h"

namespace tomato
{
    // Registry support function per collider
    EnumArray<ColliderType, GJK::SupportFunc> GJK::supportFunctions_
    {
        {ColliderType::Cube, support::Cube},
        {ColliderType::Sphere, support::Sphere},
//        {ColliderType::Capsule, support::Capsule}
    };

    std::optional<ContactData> GJK::EvaluateContactPair(entt::registry& reg, const ContactPair& pair)
    {
        if (reg.get<ColliderComponent>(pair.a).trigger ||
            reg.get<ColliderComponent>(pair.b).trigger)
        {
            if (GJKBool(reg, pair))
                return ContactData{};

            return std::nullopt;
        }

        if (auto distanceRes = GJKDistance(reg, pair))
        {
            if (distanceRes->distance <= COLLISION_SKIN + EPSILON_SQ)
                return ContactData{distanceRes->normal, distanceRes->distance};

//            std::cout << "           dist: " << distanceRes->distance << "\n";
            return GJKRaycast(reg, pair);
        }

        return std::nullopt;
    }

    glm::vec3 GJK::GetSupportPoint(
                const glm::vec3& worldDir,
                const ColliderComponent& col1, TransformComponent& trf1,
                const ColliderComponent& col2, TransformComponent& trf2)
    {
        return Support(worldDir, col1, trf1) - Support(-worldDir, col2, trf2);
    }

    bool GJK::GJKBool(
            entt::registry& reg, const ContactPair& pair)
    {
//        std::cout << "========== GJK bool " << pair << "\n";

        auto& col1 = reg.get<ColliderComponent>(pair.a);
        auto& col2 = reg.get<ColliderComponent>(pair.b);
        auto& trf1 = reg.get<TransformComponent>(pair.a);
        auto& trf2 = reg.get<TransformComponent>(pair.b);

        static std::vector<glm::vec3> simplex(4);
        simplex.clear();

        glm::vec3 closestP = GetSupportPoint(
            trf1.GetWorldPosition() - trf2.GetWorldPosition(),
            col1, trf1, col2, trf2);
        glm::vec3 supportP = GetSupportPoint(
            -closestP,
            col1, trf1, col2, trf2);
        simplex.push_back(supportP);

        int iteration = 0;
        while (iteration++ < 20) {
            if (auto result = FindClosestPointOnSimplex(simplex))
                closestP = *result;
            else
                return true;

            supportP = GetSupportPoint(-closestP, col1, trf1, col2, trf2);
            simplex.push_back(supportP);
            if (glm::dot(-closestP, supportP) < EPSILON_SQ)
                return false;   // 심플렉스가 원점에 거의 접근하는데 포함은 못하는 상황
        }
        return true;
    }

    std::optional<DistanceResult> GJK::GJKDistance(
            entt::registry& reg, const ContactPair& pair)
    {
//        std::cout << "========== GJK distance " << pair << "\n";

        auto& col1 = reg.get<ColliderComponent>(pair.a);
        auto& col2 = reg.get<ColliderComponent>(pair.b);
        auto& trf1 = reg.get<TransformComponent>(pair.a);
        auto& trf2 = reg.get<TransformComponent>(pair.b);

        static std::vector<glm::vec3> simplex(4);
        simplex.clear();

        glm::vec3 closestP = GetSupportPoint(
            trf1.GetWorldPosition() - trf2.GetWorldPosition(),
            col1, trf1, col2, trf2);
        glm::vec3 supportP = GetSupportPoint(
            -closestP,
            col1, trf1, col2, trf2);

        int iteration = 0;
        while (glm::length2(closestP) - glm::dot(closestP, supportP) > 1e-6f
            && iteration++ < 10)
        {
            simplex.push_back(supportP);

            if (auto result = FindClosestPointOnSimplex(simplex))
                closestP = *result;
            else
                break;

            supportP = GetSupportPoint(-closestP, col1, trf1, col2, trf2);
        }

        auto length = glm::length(closestP);
        if (length > EPSILON)
        {
            if (-EPSILON < closestP.x && closestP.x < EPSILON)
                closestP.x = 0.f;
            if (-EPSILON < closestP.z && closestP.z < EPSILON)
                closestP.z = 0.f;
            if (-EPSILON < closestP.y && closestP.y < EPSILON)
                closestP.y = 0.f;

            auto normal = closestP / length;
//            std::cout << " *** GJK *** " << glm::to_string(normal) << " " << length << "\n";
            return DistanceResult{-normal, length};
        }

        return RunEPA(simplex, col1, trf1, col2, trf2);
    }

    std::optional<ContactData> GJK::GJKRaycast(
            entt::registry& reg, const ContactPair& pair)
    {
//        std::cout << "========== GJK raycast " << pair << "\n";

        auto& col1 = reg.get<ColliderComponent>(pair.a);
        auto& col2 = reg.get<ColliderComponent>(pair.b);
        auto& trf1 = reg.get<TransformComponent>(pair.a);
        auto& trf2 = reg.get<TransformComponent>(pair.b);

        glm::vec3 v1{0.f};
        glm::vec3 v2{0.f};
        if (auto vel1 = reg.try_get<VelocityComponent>(GetRootEntity(reg, pair.a)))
            v1 = vel1->velocity;
        if (auto vel2 = reg.try_get<VelocityComponent>(GetRootEntity(reg, pair.b)))
            v2 = vel2->velocity;

        glm::vec3 relVel = v1 - v2;
        glm::vec3 ray = -relVel * FIXED_DELTA_TIME;

        float hitFraction = 0.f;
        glm::vec3 rayOrigin{0.f};
        glm::vec3 curRayPos = rayOrigin;
        glm::vec3 searchDir = curRayPos - GetSupportPoint(ray, col1, trf1, col2, trf2);  // CSO → curRayPos
        glm::vec3 hitNormal = searchDir;
        std::vector<glm::vec3> simplex;

        float maxDistSq = 1.f;
        int iteration = 0;
        while (glm::length2(searchDir) > EPSILON_SQ * maxDistSq
            && iteration++ < 20)
        {
            glm::vec3 supportP = GetSupportPoint(searchDir, col1, trf1, col2, trf2);
            glm::vec3 supportToRay = curRayPos - supportP;                         // 새로 얻은 심플렉스 점 → curRayPos

            float dotVW = glm::dot(searchDir, supportToRay);
            // std::cout << "     (" << iteration << ") dotVW: " << dotVW << "\n";
            if (dotVW > 0)
            {
                // 새로 얻은 심플렉스 점이 아직 curRayPos에 미치지 못함
                // curRayPos가 아직 CSO 외부에 있으므로 ray 전진 가능

                float dotVR = glm::dot(searchDir, ray);
                // std::cout << "     (" << iteration << ") dotVR: " << dotVR << "\n";
                if (dotVR >= -1e-5f)
                {
                    // Ray와 CSO가 같은 방향(평행) 또는 수직으로 멀어짐
                    // Ray를 계속 전진시켜도 CSO에 닿을 수 없음
                    return std::nullopt;
                }

                // Ray가 CSO를 향하므로 ray를 전진
                hitFraction -= dotVW / dotVR;
                if (hitFraction > 1)
                {
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
            else
            {
                TMT_WARN << "Incorrect simplex";
                break;
            }

            maxDistSq = EPSILON_SQ;
            for (auto& p : simplex)
                maxDistSq = std::max(maxDistSq, glm::length2(curRayPos - p));

//             std::cout << "     (" << iteration << ") hitNormal: " << glm::to_string(hitNormal) << "\n";
//             std::cout << "     (" << iteration << ") searchDir: " << glm::to_string(searchDir) << "\n";
//             std::cout << "     (" << iteration << ")   length2: " << glm::length2(searchDir) << "\n";
        }

        if (hitFraction <= 1)
        {
            if (glm::length2(hitNormal) > EPSILON_SQ)
            {
//                std::cout << "     bef normalize: " << glm::to_string(hitNormal) << "\n";

                if (-EPSILON < hitNormal.x && hitNormal.x < EPSILON)
                    hitNormal.x = 0.f;
                if (-EPSILON < hitNormal.z && hitNormal.z < EPSILON)
                    hitNormal.z = 0.f;
                if (-EPSILON < hitNormal.y && hitNormal.y < EPSILON)
                    hitNormal.y = 0.f;

//                std::cout << "     eps normalize: " << glm::to_string(hitNormal) << "\n";
                hitNormal = glm::normalize(hitNormal);
//                std::cout << "     aft normalize: " << glm::to_string(hitNormal) << "\n";
            }

            return ContactData{hitNormal, hitFraction, 0.f};
        }

        return std::nullopt;
    }

    glm::vec3 GJK::Support(
            const glm::vec3& worldDir,
            const ColliderComponent& col, TransformComponent& trf)
    {
        const auto worldRot = glm::toMat4(trf.GetWorldQuaternion());

        const glm::vec3 localDir = glm::transpose(worldRot) * glm::vec4(worldDir, 0.f);

        const auto localSupportP = supportFunctions_[col.type](localDir, trf);
        return trf.GetWorldPosition() + glm::vec3{worldRot * glm::vec4(localSupportP, 1.f)};
    }

    std::optional<glm::vec3> GJK::FindClosestPointOnSimplex(std::vector<glm::vec3>& simplex)
    {
        switch (simplex.size())
        {
            case 1:
                return simplex[0];

            case 2:
            {
                const auto ao = -simplex[0];
                const auto ab = simplex[1] - simplex[0];

                auto t = glm::dot(ao, ab);
                if (t <= 0)
                {
                    simplex.pop_back();
                    return simplex[0];
                }
                else {
                    float denom = glm::length2(ab);

                    if (t >= denom)
                    {
                        simplex.erase(simplex.begin());
                        return simplex[0];
                    }
                    else
                    {
                        t /= denom;
                        return simplex[0] + t * ab;
                    }
                }
            }

            case 3:
                return FindClosestPointOnTriangle(simplex);

            case 4:
            {
                glm::vec3 p{0.f};
                glm::vec3 a = simplex[0];
                glm::vec3 b = simplex[1];
                glm::vec3 c = simplex[2];
                glm::vec3 d = simplex[3];

                glm::vec3 closestP = p;
                float bestSqDist = std::numeric_limits<float>::max();
                std::vector<glm::vec3> bestSimplex;

                // ABC
                if (PointOutsideOfPlane(p, a, b, c, d))
                {
                    auto q = ClosestPtPointTriangle(p, a, b, c);
                    float sqDist = glm::length2(q - p);

                    if (sqDist < bestSqDist)
                    {
                        bestSqDist = sqDist;
                        closestP = q;
                        bestSimplex = {a, b, c};
                    }
                }

                // ACD
                if (PointOutsideOfPlane(p, a, c, d, b))
                {
                    auto q = ClosestPtPointTriangle(p, a, c, d);
                    float sqDist = glm::length2(q - p);
                    if (sqDist < bestSqDist)
                    {
                        bestSqDist = sqDist;
                        closestP = q;
                        bestSimplex = {a, c, d};
                    }
                }

                // ADB
                if (PointOutsideOfPlane(p, a, d, b, c))
                {
                    auto q = ClosestPtPointTriangle(p, a, d, b);
                    float sqDist = glm::length2(q - p);
                    if (sqDist < bestSqDist)
                    {
                        bestSqDist = sqDist;
                        closestP = q;
                        bestSimplex = {a, d, b};
                    }
                }

                // BDC
                if (PointOutsideOfPlane(p, b, d, c, a))
                {
                    auto q = ClosestPtPointTriangle(p, b, d, c);
                    float sqDist = glm::length2(q - p);
                    if (sqDist < bestSqDist)
                    {
                        bestSqDist = sqDist;
                        closestP = q;
                        bestSimplex = {b, d, c};
                    }
                }

                if (bestSimplex.empty())
                    return std::nullopt;
                else
                {
                    simplex = bestSimplex;
                    return closestP;
                }
            }

            default:
                TMT_ERR << "Incorrect simplex size: " << simplex.size();
                return glm::vec3{0.f};
        }
    }

    glm::vec3 GJK::FindClosestPointOnTriangle(std::vector<glm::vec3>& simplex)
    {
        const glm::vec3 a = simplex[0];
        const glm::vec3 b = simplex[1];
        const glm::vec3 c = simplex[2];

        // Vertex A region
        const glm::vec3 ab = b - a;
        const glm::vec3 ac = c - a;
        const glm::vec3 ao = -a;

        const float d1 = glm::dot(ab, ao);
        const float d2 = glm::dot(ac, ao);
        if (d1 <= 0.f && d2 <= 0.f)
        {
            simplex = {a};
            return a;
        }

        // Vertex B region
        glm::vec3 bo = -b;

        const float d3 = glm::dot(ab, bo);
        const float d4 = glm::dot(ac, bo);
        if (d3 >= 0.f && d4 <= d3)
        {
            simplex = {b};
            return b;
        }

        // Edge AB region
        const float vc = d1 * d4 - d3 * d2;
        if (vc <= 0.f && d1 >= 0.f && d3 <= 0.f)
        {
            float v = d1 / (d1 - d3);
            simplex = {a, b};
            return a + v * ab;
        }

        // Vertex C region
        const glm::vec3 co = -c;

        const float d5 = glm::dot(ab, co);
        const float d6 = glm::dot(ac, co);
        if (d6 >= 0.f && d5 <= d6)
        {
            simplex = {c};
            return c;
        }

        // Edge AC region
        const float vb = d5 * d2 - d1 * d6;
        if (vb <= 0.f && d2 >= 0.f && d6 <= 0.f)
        {
            float w = d2 / (d2 - d6);
            simplex = {a, c};
            return a + w * ac;
        }

        // Edge BC region
        const float va = d3 * d6 - d5 * d4;
        if (va <= 0.f && (d4 - d3) >= 0.f && (d5 - d6) >= 0.f)
        {
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
            const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
    {
        // Vertex A region
        const glm::vec3 ab = b - a;
        const glm::vec3 ac = c - a;
        const glm::vec3 ap = p - a;

        const float d1 = glm::dot(ab, ap);
        const float d2 = glm::dot(ac, ap);
        if (d1 <= 0.f && d2 <= 0.f)
            return a;

        // Vertex B region
        glm::vec3 bp = p - b;

        const float d3 = glm::dot(ab, bp);
        const float d4 = glm::dot(ac, bp);
        if (d3 >= 0.f && d4 <= d3)
            return b;

        // Edge AB region
        const float vc = d1 * d4 - d3 * d2;
        if (vc <= 0.f && d1 >= 0.f && d3 <= 0.f)
        {
            float v = d1 / (d1 - d3);
            return a + v * ab;
        }

        // Vertex C region
        const glm::vec3 cp = p - c;

        const float d5 = glm::dot(ab, cp);
        const float d6 = glm::dot(ac, cp);
        if (d6 >= 0.f && d5 <= d6)
            return c;

        // Edge AC region
        const float vb = d5 * d2 - d1 * d6;
        if (vb <= 0.f && d2 >= 0.f && d6 <= 0.f)
        {
            float w = d2 / (d2 - d6);
            return a + w * ac;
        }

        // Edge BC region
        const float va = d3 * d6 - d5 * d4;
        if (va <= 0.f && (d4 - d3) >= 0.f && (d5 - d6) >= 0.f)
        {
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
            const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
    {
        return glm::dot(p - a, glm::cross(b - a, c - a)) >= 0.f;
    }

    int GJK::PointOutsideOfPlane(
            const glm::vec3& p,
            const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d)
    {
        float signp = glm::dot(p - a, glm::cross(b - a, c - a));
        float signd = glm::dot(d - a, glm::cross(b - a, c - a));
        //return signp * signd < 0.f;
        return signp * signd < 1e-6f;
    }
}
