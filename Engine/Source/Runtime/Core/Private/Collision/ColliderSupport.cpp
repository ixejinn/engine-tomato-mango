#include <glm/glm.hpp>
#include <limits>
#include "Collision/ColliderSupport.h"
#include "ECS/Components/Collision.h"

namespace tomato::support {
    glm::vec3 Cube(const glm::vec3 &dir, const TransformComponent &trf) {
        auto halfExtents = trf.GetLocalScale() * 0.5f;
        return trf.GetLocalPosition() + glm::vec3{
            dir.x >= 0.f ? halfExtents.x : -halfExtents.x,
            dir.y >= 0.f ? halfExtents.y : -halfExtents.y,
            dir.z >= 0.f ? halfExtents.z : -halfExtents.z
        };
    }

    glm::vec3 Sphere(const glm::vec3 &dir, const TransformComponent &trf) {
        float dirLenSq = glm::length2(dir);
        if (dirLenSq < 1e-4)
            return trf.GetLocalPosition();
        return trf.GetLocalPosition() + (trf.GetLocalScale().x * 0.5f) * (dir / std::sqrt(dirLenSq));
    }

    glm::vec3 Capsule(const glm::vec3 &dir, const TransformComponent &trf) {
        float dirLenSq = glm::length2(dir);
        if (dirLenSq < 1e-4)
            return trf.GetLocalPosition();

        auto localPos = trf.GetLocalPosition();
        auto halfExtents = trf.GetLocalScale() * 0.5f;
        const glm::vec3 p1 = localPos + glm::vec3(0, halfExtents.y, 0);
        const glm::vec3 p2 = localPos - glm::vec3(0, halfExtents.y, 0);
        return (glm::dot(dir, p1) > glm::dot(dir, p2) ? p1 : p2) + halfExtents.x * (dir / std::sqrt(dirLenSq));
    }
}
