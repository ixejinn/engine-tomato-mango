#include <glm/glm.hpp>
#include <limits>
#include "Collision/ColliderSupport.h"
#include "ECS/Components/Collision.h"

namespace tomato::support {
    glm::vec3 Cube(const glm::vec3 &dir, const ColliderComponent &col) {
        return col.position + glm::vec3{
            dir.x >= 0.f ? col.halfExtents.x : -col.halfExtents.x,
            dir.y >= 0.f ? col.halfExtents.y : -col.halfExtents.y,
            dir.z >= 0.f ? col.halfExtents.z : -col.halfExtents.z
        };
    }

    glm::vec3 Sphere(const glm::vec3 &dir, const ColliderComponent &col) {
        float dirLenSq = glm::length2(dir);
        if (dirLenSq < 1e-4)
            return col.position;
        return col.position + col.halfExtents.x * (dir / std::sqrt(dirLenSq));
    }

    glm::vec3 Capsule(const glm::vec3 &dir, const ColliderComponent &col) {
        float dirLenSq = glm::length2(dir);
        if (dirLenSq < 1e-4)
            return col.position;

        const glm::vec3 p1 = col.position + glm::vec3(0, col.halfExtents.y, 0);
        const glm::vec3 p2 = col.position - glm::vec3(0, col.halfExtents.y, 0);
        return (glm::dot(dir, p1) > glm::dot(dir, p2) ? p1 : p2) + col.halfExtents.x * (dir / std::sqrt(dirLenSq));
    }
}
