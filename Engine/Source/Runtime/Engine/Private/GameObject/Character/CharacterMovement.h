#ifndef MANGO_CHARACTERMOVEMENT_H
#define MANGO_CHARACTERMOVEMENT_H

#include "entt/fwd.hpp"
#include "ECS/Forward/CharacterCompFwd.h"
#include "Collision/CollisionEventFwd.h"
#include "GameObject/Character/MovementMode.h"

namespace tomato::CharacterMovement
{
    void OnTriggerEnter_UpdateMovementMode(TriggerEnterEvent& event);
    void OnTriggerExit_UpdateMovementMode(TriggerExitEvent& event);

    void Jump(entt::registry& reg, entt::entity e, float jumpSpeed); // move to public?
    void Jump(MovementComponent& move, VelocityComponent& vel, float jumpSpeed);

    void Land(MovementComponent& move, VelocityComponent& vel);

    void ChangeMovementMode(MovementComponent& move, MovementMode mode);
}

#endif //MANGO_CHARACTERMOVEMENT_H
