#ifndef MANGO_RIGIDBODY_H
#define MANGO_RIGIDBODY_H

#include <glm/vec3.hpp>

namespace tomato
{
    struct VelocityComponent
    {
        glm::vec3 velocity;
        glm::vec3 input;

        float horizontalSpeed{ 2.f };

        glm::vec3 Get() const { return velocity + input; }
    };
}

#endif //MANGO_RIGIDBODY_H