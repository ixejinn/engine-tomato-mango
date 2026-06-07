#include "Prefab/Character/CharacterMovement.h"
#include "Collision/CollisionEvent.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/Rigidbody.h"
#include "Utils/Logger.h"

namespace tomato::CharacterMovement {
    void StartFalling(const TriggerExitEvent& event, entt::entity e) {
        auto& move = event.reg->get<MovementComponent>(GetRootEntity(event.reg, e));
        move.mode = MovementMode::Falling;

        TMT_INFO << "Falling";
    }

    void AfterLanding(const TriggerEnterEvent& event, entt::entity e) {
        entt::entity rootEntity = GetRootEntity(event.reg, e);

        auto& move = event.reg->get<MovementComponent>(rootEntity);
        move.mode = MovementMode::Walking;
        move.jumpCnt = 0;

        auto& velocity = event.reg->get<VelocityComponent>(rootEntity);
        velocity.velocity.y = 0;

        TMT_INFO << "Walking";
    }
}