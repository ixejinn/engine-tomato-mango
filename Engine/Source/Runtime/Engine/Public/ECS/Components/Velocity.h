#ifndef MANGO_VELOCITY_H
#define MANGO_VELOCITY_H

#include <glm/vec3.hpp>

namespace tomato
{
    struct VelocityComponent
    {
        glm::vec3 velocity;
        glm::vec3 input;

        float horizontalSpeed{2.0f};
        float acceleration{10.0f};

        glm::vec3 Get() const { return velocity + input; }
    };

    struct FrictionComponent
    {
        float groundFriction{1.0f};
        float airDrag{0.5f};
    };

    struct SurfaceComponent
    {
        float friction{1.0f};
    };
}

#endif //MANGO_VELOCITY_H