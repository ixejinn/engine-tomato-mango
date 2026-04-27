#ifndef MANGO_COLLIDERSUPPORT_H
#define MANGO_COLLIDERSUPPORT_H

#include <glm/fwd.hpp>
#include "ECS/PhysCompFwd.h"

namespace tomato::support {
    glm::vec3 Cube(const glm::vec3& dir, const ColliderComponent& col);
    glm::vec3 Sphere(const glm::vec3& dir, const ColliderComponent& col);
    glm::vec3 Capsule(const glm::vec3& dir, const ColliderComponent& col);
}

#endif //MANGO_COLLIDERSUPPORT_H
