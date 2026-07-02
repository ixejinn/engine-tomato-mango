#ifndef MANGO_MOVEMENTMODE_H
#define MANGO_MOVEMENTMODE_H

#include <entt/fwd.hpp>

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
        entt::registry* reg;
        MovementMode mode;
    };
}

#endif //MANGO_MOVEMENTMODE_H