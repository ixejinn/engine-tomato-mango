#include "Prefab/Character/CharacterMovement.h"
#include "Collision/CollisionEvent.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/Rigidbody.h"
#include "ECS/Entity/Hierarchy.h"
#include "Utils/Logger.h"

namespace tomato::CharacterMovement {
    void StartFalling(const TriggerExitEvent& event, entt::entity e) {
        entt::entity root = GetRootEntity(event.reg, e);

        auto& move = event.reg->get<MovementComponent>(root);
        if (--move.gndStayCnt == 0) {
            move.mode = MovementMode::Falling;
            TMT_INFO << "Falling " << (int)e;
        }
    }

    void AfterLanding(const TriggerEnterEvent& event, entt::entity e) {
        entt::entity other = event.e1 == e ? event.e2 : event.e1;
        entt::entity root = GetRootEntity(event.reg, e);

        auto& move = event.reg->get<MovementComponent>(root);
        ++move.gndStayCnt;

        move.mode = MovementMode::Walking;
        move.jumpCnt = 0;

        auto& velocity = event.reg->get<VelocityComponent>(root);
        velocity.velocity.y = 0;

        TMT_INFO << "Walking " << (int)e << " " << event.tick;
    }
}