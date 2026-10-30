#ifndef MANGO_VELOCITY_H
#define MANGO_VELOCITY_H

#include <glm/vec3.hpp>

namespace tomato
{
    struct VelocityComponent
    {
        glm::vec3 velocity{0};
        glm::vec3 input{0};

        float horizontalSpeed{3};
        float acceleration{20};

        glm::vec3 Get() const { return velocity + input; }
    };

    struct FrictionComponent
    {
        float groundFriction{5};
        float airDrag{0.5};
    };

    struct SurfaceComponent
    {
        float friction{1};
    };
}

#endif //MANGO_VELOCITY_H