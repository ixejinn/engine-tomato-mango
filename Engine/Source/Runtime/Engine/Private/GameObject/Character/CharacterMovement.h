#ifndef MANGO_CHARACTERMOVEMENT_H
#define MANGO_CHARACTERMOVEMENT_H

#include "entt/fwd.hpp"
#include "Collision/CollisionEventFwd.h"
#include "GameObjects/Character/MovementMode.h"

namespace tomato::CharacterMovement
{
    void ChangeMovementMode(ChangeMovementModeEvent& event);
}

#endif //MANGO_CHARACTERMOVEMENT_H
