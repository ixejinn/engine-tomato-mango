#include "CharacterMovement.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/ComponentsPhys.h"
#include "ECS/Entity/Hierarchy.h"
#include "Collision/CollisionEvent.h"
#include "Particle/ParticleEmitterPool.h"
#include "Event/EventDispatcher.h"
#include "State/State.h"
#include "Utils/Logger.h"
#include "Resource/AssetHash.h"

namespace tomato::CharacterMovement
{
    void OnTriggerEnter_UpdateMovementMode(const TriggerEnterEvent& event)
    {
        auto* move1 = event.reg->try_get<MovementComponent>(event.e1);
        auto* move2 = event.reg->try_get<MovementComponent>(event.e2);

        auto* vel1 = event.reg->try_get<VelocityComponent>(event.e1);
        auto* vel2 = event.reg->try_get<VelocityComponent>(event.e2);

        if (move1 && vel1 && event.reg->get<ColliderComponent>(event.e1).isTrigger)
        {
//            EventDispatcher::GetInstance().Enqueue(LandingEvent{
//                event.e1, event.reg->, registry.get<TransformComponent>(event.e).GetWorldPosition()});
            Land(*move1, *vel1);
        }

        if (move2 && vel2 && event.reg->get<ColliderComponent>(event.e2).isTrigger)
            ChangeMovementMode(*move2, Walking);
    }

    void OnTriggerExit_UpdateMovementMode(const TriggerExitEvent& event)
    {
        auto* move1 = event.reg->try_get<MovementComponent>(event.e1);
        auto* move2 = event.reg->try_get<MovementComponent>(event.e2);

        if (move1 && event.reg->get<ColliderComponent>(event.e1).isTrigger)
            ChangeMovementMode(*move1, Falling);

        if (move2 && event.reg->get<ColliderComponent>(event.e2).isTrigger)
            ChangeMovementMode(*move2, Falling);
    }

    void Jump(entt::registry& reg, entt::entity e, float jumpSpeed)
    {
        auto* move = reg.try_get<MovementComponent>(e);
        auto* vel = reg.try_get<VelocityComponent>(e);

        if (move && vel)
            Jump(*move, *vel, jumpSpeed);
    }

    void Jump(MovementComponent& move, VelocityComponent& vel, float jumpSpeed)
    {
        vel.velocity.y = std::max(vel.velocity.y, 0.f) + jumpSpeed;

        ++move.jumpCnt;
        ChangeMovementMode(move, Falling);
    }

    void Land(MovementComponent& move, VelocityComponent& vel)
    {
        ++move.gndStayCnt;
        move.jumpCnt = 0;

        ChangeMovementMode(move, Walking);
    }

    void ChangeMovementMode(MovementComponent& move, MovementMode mode)
    {
        switch (mode)
        {
        case Falling:
            {
                if (--move.gndStayCnt == 0)
                {
                    move.mode = Falling;
                    // TMT_INFO << "Falling";
                }
            }
            break;
        case Walking:
            {
                TMT_DEBUG << "WORK IN PROGRESS :: CharacterMovement";
//                entt::entity root = GetRootEntity(registry, event.e);
//                auto& move = registry.get<MovementComponent>(root);
//                ++move.gndStayCnt;
//
//                EventDispatcher::GetInstance().Enqueue(LandingEvent{event.e, event.state, registry.get<TransformComponent>(event.e).GetWorldPosition()});
//
//                move.mode = Walking;
//                move.jumpCnt = 0;
//
//                registry.get<VelocityComponent>(root).velocity.y = 0;
//                TMT_INFO << "Walking " << (int)event.e;
            }
            break;
        }
    }
}