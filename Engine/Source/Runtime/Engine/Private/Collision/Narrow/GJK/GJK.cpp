#define GLM_ENABLE_EXPERIMENTAL
#include <cmath>
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
        const auto& colA = reg.get<ColliderComponent>(pair.a);
        const auto& colB = reg.get<ColliderComponent>(pair.b);

        if (colA.trigger || colB.trigger)
        {
            if (GJKBool(reg, pair))
                return ContactData{};

            return std::nullopt;
        }

        if (auto distanceRes = GJKDistance(reg, pair))
        {
            constexpr float SAFETY = 1.28f;
            if (distanceRes->maxDistSq < 0 ||   // EPA
                distanceRes->distance * distanceRes->distance <= SAFETY * SAFETY * RELATIVE_TOLERANCE_SQ * distanceRes->maxDistSq)
                return ContactData{distanceRes->normal, distanceRes->distance};

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

        float maxDistSq = glm::length2(supportP);
        int iteration = 0;
        while (iteration++ < 20) {
            if (auto result = FindClosestPointOnSimplex(simplex))
                closestP = *result;
            else
                return true;

            supportP = GetSupportPoint(-closestP, col1, trf1, col2, trf2);
            simplex.push_back(supportP);

            maxDistSq = std::max(maxDistSq, glm::length2(supportP));
            if (glm::dot(-closestP, supportP) < RELATIVE_TOLERANCE_SQ * maxDistSq)
                return false;   // 심플렉스를 더 수렴시켜도 원점에 도달할 수 없으므로 비충돌
        }
        return true;
    }

    std::optional<DistanceResult> GJK::GJKDistance(
            entt::registry& reg, const ContactPair& pair)
    {
        // std::cout << "========== GJK distance " << pair << "\n";

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

        float maxDistSq = glm::length2(closestP);
        int iteration = 0;
        while (glm::length2(closestP) - glm::dot(closestP, supportP) > RELATIVE_TOLERANCE_SQ * maxDistSq
            && iteration++ < 20)
        {
            simplex.push_back(supportP);

            if (auto result = FindClosestPointOnSimplex(simplex))
                closestP = *result;
            else
                return RunEPA(simplex, col1, trf1, col2, trf2);

            supportP = GetSupportPoint(-closestP, col1, trf1, col2, trf2);

            maxDistSq = 0;
            for (const glm::vec3& p : simplex)
                maxDistSq = std::max(maxDistSq, glm::length2(p));
        }

        const float lengthSq = glm::length2(closestP);
        if (lengthSq > RELATIVE_TOLERANCE_SQ * maxDistSq)
        {
            const float length = glm::sqrt(lengthSq);
            auto normal = closestP / length;

            if (std::abs(normal.x) < NORMAL_SNAP_THRESHOLD)
                closestP.x = 0.f;
            if (std::abs(normal.z) < NORMAL_SNAP_THRESHOLD)
                closestP.z = 0.f;
            if (std::abs(normal.y) < NORMAL_SNAP_THRESHOLD)
                closestP.y = 0.f;

            normal = glm::normalize(normal);

            //                       ↓ normal 방향이 원점에서 CSO를 향하는 방향이므로 raycast, EPA와 방향을 맞추기위해 부호 반전
            return DistanceResult{-normal, length, maxDistSq};
        }

        return RunEPA(simplex, col1, trf1, col2, trf2);
    }

    std::optional<ContactData> GJK::GJKRaycast(
            entt::registry& reg, const ContactPair& pair)
    {
        // std::cout << "========== GJK raycast " << pair << "\n";

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
        glm::vec3 searchDir = curRayPos - GetSupportPoint(ray, col1, trf1, col2, trf2);
        glm::vec3 hitNormal = searchDir;
        std::vector<glm::vec3> simplex;  // curRayPos - supportP 상대 좌표를 저장
        simplex.reserve(4);

        float maxDistSq = glm::length2(searchDir);
        int iteration = 0;
        while (glm::length2(searchDir) > RELATIVE_TOLERANCE_SQ * maxDistSq // searchDir의 크기가 충분히 큰 경우에만 반복
            && iteration++ < 20)
        {
            glm::vec3 supportP = GetSupportPoint(searchDir, col1, trf1, col2, trf2);    // (절대)
            glm::vec3 supportToRay = curRayPos - supportP;                                    // (상대 - curRayPos 기준)

            // searchDir에 수직하고 supportP를 포함하는 평면이 CSO와 curRayPos를 분리시키면
            // curRayPos를 평면(CSO와 충돌하지 않는 하한)까지 ray 방향으로 전진시킬 수 있음
            float separationOnAxis = glm::dot(searchDir, supportToRay);
            if (separationOnAxis >  0)  // 평면이 CSO와 curRayPos를 분리
            {
                float rayOnAxis = glm::dot(searchDir, ray);
                if (rayOnAxis >= 0)     // ray가 searchDir과 평행하거나 수직하여 평면을 만날 수 없음
                {
                    return std::nullopt;
                }

                hitFraction -= separationOnAxis / rayOnAxis;
                if (hitFraction > 1)    // 이번 틱에서 충돌하지 않음
                {
                    return std::nullopt;
                }

                // ray 전진
                glm::vec3 preRayPos = curRayPos;
                curRayPos = rayOrigin + hitFraction * ray;

                // 상대 좌표 보정
                glm::vec3 deltaRay = curRayPos - preRayPos;
                for (auto& p : simplex)
                    p += deltaRay;

                // 노말 벡터 갱신
                hitNormal = searchDir;
            }

            simplex.push_back(curRayPos - supportP);
            if (auto result = FindClosestPointOnSimplex(simplex))
                searchDir = result.value();
            else
            {
                TMT_DEBUG << "Degenerate simplex. iter = " << iteration;
                break;
            }

            maxDistSq = 0.f;
            for (auto& p : simplex)
                maxDistSq = std::max(maxDistSq, glm::length2(p));
        }

        if (hitFraction > 0 && hitFraction <= 1)
        {
            hitNormal = glm::normalize(hitNormal);

            if (std::abs(hitNormal.x) < NORMAL_SNAP_THRESHOLD)
                hitNormal.x = 0.f;
            if (std::abs(hitNormal.z) < NORMAL_SNAP_THRESHOLD)
                hitNormal.z = 0.f;
            if (std::abs(hitNormal.y) < NORMAL_SNAP_THRESHOLD)
                hitNormal.y = 0.f;

            hitNormal = glm::normalize(hitNormal);

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
        const glm::vec3 n = glm::cross(b - a, c - a);
        float signp = glm::dot(p - a, n);
        float signd = glm::dot(d - a, n);
        // return signp * signd < 0;
        return (signp < 0) != (signd < 0);
    }
}
