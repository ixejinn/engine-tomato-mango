#include <glm/glm.hpp>
#include "Collision/ColliderSupport.h"
#include "Collision/CollisionTolerance.h"
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
        if (dirLenSq < EPSILON_SQ)
        {
            offset = glm::vec3
            {
                dir.x >= 0.f ? 1.f : -1.f,
                dir.y >= 0.f ? 1.f : -1.f,
                dir.z >= 0.f ? 1.f : -1.f
            };
            offset = glm::normalize(offset);
        }
        else
            offset = (dir / std::sqrt(dirLenSq));
        return (trf.GetWorldScale().x * 0.5f) * offset;
    }

    glm::vec3 Capsule(const glm::vec3 &dir, const TransformComponent &trf)
    {
        float dirLenSq = glm::length2(dir);
        glm::vec3 offset;
        if (dirLenSq < EPSILON_SQ)
        {
            offset = glm::vec3
            {
                dir.x >= 0.f ? 1.f : -1.f,
                dir.y >= 0.f ? 1.f : -1.f,
                dir.z >= 0.f ? 1.f : -1.f
            };
            offset = glm::normalize(offset);
        }
        else
            offset = (dir / std::sqrt(dirLenSq));

        const glm::vec3 center{0, glm::sign(offset.y), 0};
        const glm::vec3 wHalfScl = trf.GetWorldScale() * 0.5f;
        return (center * wHalfScl.y) + offset * wHalfScl.x;
    }
}
