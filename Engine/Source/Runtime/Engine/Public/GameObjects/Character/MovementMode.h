#ifndef MANGO_MOVEMENTMODE_H
#define MANGO_MOVEMENTMODE_H

#include <entt/fwd.hpp>
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
}

#endif //MANGO_MOVEMENTMODE_H