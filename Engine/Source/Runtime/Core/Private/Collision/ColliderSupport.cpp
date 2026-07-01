#include <glm/glm.hpp>
#include "Collision/ColliderSupport.h"
#include "ECS/Components/Collision.h"
#include "ECS/Components/Transform.h"

namespace tomato::support
{
    glm::vec3 Cube(const glm::vec3 &dir, const TransformComponent &trf)
    {
        auto halfExtents = trf.GetWorldScale() * 0.5f;
        return glm::vec3{
            dir.x >= 0.f ? halfExtents.x : -halfExtents.x,
            dir.y >= 0.f ? halfExtents.y : -halfExtents.y,
            dir.z >= 0.f ? halfExtents.z : -halfExtents.z
        };
    }

    glm::vec3 Sphere(const glm::vec3 &dir, const TransformComponent &trf)
    {
        float dirLenSq = glm::length2(dir);
        glm::vec3 offset;
        if (dirLenSq < 1e-4)
        {
            offset = glm::vec3
            {
                dir.x >= 0.f ? 1.f : -1.f,
                dir.y >= 0.f ? 1.f : -1.f,
                dir.z >= 0.f ? 1.f : -1.f
            };
        }
        else
            offset = (dir / std::sqrt(dirLenSq));
        return (trf.GetWorldScale().x * 0.5f) * offset;
    }

    glm::vec3 Capsule(const glm::vec3 &dir, const TransformComponent &trf)
    {
        float dirLenSq = glm::length2(dir);
        if (dirLenSq < 1e-4)
            return glm::vec3{0.f};

        auto halfExtents = trf.GetWorldScale() * 0.5f;
        const glm::vec3 p1 =  glm::vec3(0, halfExtents.y, 0);
        const glm::vec3 p2 = -glm::vec3(0, halfExtents.y, 0);
        return (glm::dot(dir, p1) > glm::dot(dir, p2) ? p1 : p2) + halfExtents.x * (dir / std::sqrt(dirLenSq));
    }
}
