#ifndef MANGO_CHARACTERMOVEMENT_H
#define MANGO_CHARACTERMOVEMENT_H

#include <entt/fwd.hpp>
#include "Collision/CollisionEventFwd.h"
#include "GameObjects/Character/MovementMode.h"

namespace tomato::CharacterMovement
{
    void StartFalling(const TriggerExitEvent& event);
    void AfterLanding(const TriggerEnterEvent& event);

    void ChangeMovementMode(MovementMode mode, entt::registry* reg, entt::entity e);
}

#endif //MANGO_CHARACTERMOVEMENT_H
