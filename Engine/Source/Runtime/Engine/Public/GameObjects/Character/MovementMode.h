#ifndef MANGO_MOVEMENTMODE_H
#define MANGO_MOVEMENTMODE_H

#include <entt/fwd.hpp>
#include <glm/vec3.hpp>
#include "State/StateFwd.h"

namespace tomato
{
    enum MovementMode
    {
        Walking,
        Falling
    };

    struct ChangeMovementModeEvent
    {
        entt::entity e;
        State* state;
        MovementMode mode;
    };

    struct LandingEvent
    {
        entt::entity e;
        State* state;
        glm::vec3 position;
    };
}

#endif //MANGO_MOVEMENTMODE_H