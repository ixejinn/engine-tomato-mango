#include "Prefab/Character/CharacterMovement.h"
#include "Collision/CollisionEvent.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Hierarchy.h"

namespace tomato::CharacterMovement {
    void StartFalling(const TriggerExitEvent& event, entt::entity e) {
        auto& move = event.reg->get<MovementComponent>(GetRootEntity(event.reg, e));
        move.mode = MovementMode::Falling;
    }

    void AfterLanding(const TriggerEnterEvent& event, entt::entity e) {
        auto& move = event.reg->get<MovementComponent>(GetRootEntity(event.reg, e));
        move.mode = MovementMode::Walking;
    }
}