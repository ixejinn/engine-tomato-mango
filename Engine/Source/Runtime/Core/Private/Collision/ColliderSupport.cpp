#include <glm/glm.hpp>
#include <limits>
#include "Collision/ColliderSupport.h"
#include "ECS/Components/Collision.h"

namespace tomato::support {
    glm::vec3 Cube(const glm::vec3 &dir, const ColliderComponent &col) {
        auto maxDotV = std::numeric_limits<float>::lowest();
        glm::vec3 supportP;

        for (int x = 0; x <= 1; ++x)
        {
            for (int y = 0; y <= 1; ++y)
            {
                for (int z = 0; z <= 1; ++z)
                {
                    glm::vec3 p = col.position;
                    p += glm::vec3(
                            x == 0 ? col.halfExtents.x : -col.halfExtents.x,
                            y == 0 ? col.halfExtents.y : -col.halfExtents.y,
                            z == 0 ? col.halfExtents.z : -col.halfExtents.z
                    );

                    auto dotV = glm::dot(dir, p);
                    if (dotV > maxDotV)
                    {
                        maxDotV = dotV;
                        supportP = p;
                    }
                }
            }
        }

        return supportP;
    }

    glm::vec3 Sphere(const glm::vec3 &dir, const ColliderComponent &col) {
        return col.position + col.halfExtents.x * glm::normalize(dir);
    }

    glm::vec3 Capsule(const glm::vec3 &dir, const ColliderComponent &col) {
        const glm::vec3 p1 = col.position + glm::vec3(0, col.halfExtents.y, 0);
        const glm::vec3 p2 = col.position - glm::vec3(0, col.halfExtents.y, 0);
        return (glm::dot(dir, p1) > glm::dot(dir, p2) ? p1 : p2) + col.halfExtents.x * glm::normalize(dir);
    }
}
