#include "Collision/Detection/Narrow/Pairs/SphereSphere.h"
#include "Collision/ColliderSupport.h"
#include "Collision/CollisionConstants.h"
#include "Collision/CollisionTolerance.h"
#include "ECS/Components/ComponentsPhys.h"

namespace tomato::SphereSphere
{
    bool Overlap(const ShapeView& a, const ShapeView& b)
    {
        const glm::vec3 d = a.trf.GetWorldPosition() - b.trf.GetWorldPosition();
        const float r = support::SphereRadius(a.trf) + support::SphereRadius(b.trf);
        return glm::length2(d) <= r * r;
    }

    std::optional<ContactData> Sweep(const ShapeView& sphereA, const ShapeView& sphereB, const glm::vec3& ray, bool reversed)
    {
        // 두 구의 CSO는 중심이 d, 반지름이 r인 구
        const glm::vec3 d = sphereA.trf.GetWorldPosition() - sphereB.trf.GetWorldPosition();
        const float r = support::SphereRadius(sphereA.trf) + support::SphereRadius(sphereB.trf);

        const float distSq = glm::length2(d);
        const float dist = std::sqrt(distSq);   // 원점과 d 사이의 거리
        const float distance = dist - r;        // 원점과 CSO 표면 사이의 거리

        //// 겹쳐 있거나 닿아 있는 상태일 경우
        if (distance <= 0 ||
            distance * distance <= SAFETY_COEF_SQ * RELATIVE_TOLERANCE_SQ * r * r)  // GJK와 허용 오차 조건을 맞추기 위함
        {
            const glm::vec3 n = distSq > RELATIVE_TOLERANCE_SQ * r * r ? -d / dist : glm::vec3{1, 0, 0};
            return ContactData{n, distance};
        }

        //// 구(CSO)와 원점으로부터 ray 방향으로 t 만큼 이동한 점이 표면에 닿는 순간의 t 값을 구함
        // f(t) = |t·ray - d|² - r²
        //      = dot(ray, ray)t² - 2dot(ray, d)t + distSq - r²
        //      = at² + 2b't + c

        // ray와 구가 닿을 수 있는지 방향 확인
        const float bPrime = -glm::dot(ray, d);
        if (bPrime >= 0)
            return std::nullopt;

        // f(t)의 판별식 < 0이면 ray는 구와 닿지 않음
        const float a = glm::dot(ray, ray);
        const float c = distSq - r * r;
        const float disc = bPrime * bPrime - a * c;
        if (disc < 0)
            return std::nullopt;

        // f(t) = 0을 만족하는 t 중 작은 t
        const float t = (-bPrime - std::sqrt(disc)) / a;
        if (t > 1)  // 이번 틱에서 충돌하지 않음
            return std::nullopt;

        return ContactData{glm::normalize(t * ray - d), t, 0};
    }
}