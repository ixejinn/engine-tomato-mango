#ifndef MANGO_CHARACTERMOVEMENT_H
#define MANGO_CHARACTERMOVEMENT_H

#include "entt/fwd.hpp"
#include "ECS/Forward/CharacterCompFwd.h"
#include "Collision/CollisionEventFwd.h"
#include "GameObject/Character/MovementMode.h"

namespace tomato::CharacterMovement
{
    void OnTriggerEnter_UpdateMovementMode(const TriggerEnterEvent& event);
    void OnTriggerExit_UpdateMovementMode(const TriggerExitEvent& event);

    void Jump(
            entt::registry& reg, entt::entity e,
            float jumpSpeed); // move to public?
    void Jump(
            entt::registry& reg, entt::entity e,
            MovementComponent& move, VelocityComponent& vel, float jumpSpeed);

    void Land(
            entt::registry& reg, entt::entity e,
            MovementComponent& move, VelocityComponent& vel);

    void ChangeMovementMode(
            entt::registry& reg, entt::entity e,
            MovementComponent& move, MovementMode mode);
}

#endif //MANGO_CHARACTERMOVEMENT_H
