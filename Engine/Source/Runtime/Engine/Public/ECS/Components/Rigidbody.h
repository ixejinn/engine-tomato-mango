#ifndef MANGO_RIGIDBODY_H
#define MANGO_RIGIDBODY_H

#include <glm/vec3.hpp>

namespace tomato
{
    struct VelocityComponent
    {
        float horizontalSpeed{ 2.f };
        glm::vec3 velocity;
    };
}

#endif //MANGO_RIGIDBODY_H