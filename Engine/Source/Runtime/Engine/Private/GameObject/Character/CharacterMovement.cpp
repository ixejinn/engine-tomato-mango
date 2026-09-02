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
        auto root1 = GetRootEntity(event.reg, event.a);
        auto root2 = GetRootEntity(event.reg, event.b);

        auto* move1 = event.reg->try_get<MovementComponent>(root1);
        auto* move2 = event.reg->try_get<MovementComponent>(root2);

        auto* vel1 = event.reg->try_get<VelocityComponent>(root1);
        auto* vel2 = event.reg->try_get<VelocityComponent>(root2);

        if (move1 && vel1 && event.reg->get<ColliderComponent>(event.a).trigger)
        {
            EventDispatcher::GetInstance().Enqueue(
                    LandingEvent{
                        root1, event.reg, event.reg->get<TransformComponent>(root1).GetWorldPosition()});
            Land(*event.reg, root1, *move1, *vel1);
        }

        if (move2 && vel2 && event.reg->get<ColliderComponent>(event.b).trigger)
        {
            EventDispatcher::GetInstance().Enqueue(
                    LandingEvent{
                        root2, event.reg, event.reg->get<TransformComponent>(root2).GetWorldPosition()});
            Land(*event.reg, root2, *move2, *vel2);
        }
    }

    void OnTriggerExit_UpdateMovementMode(const TriggerExitEvent& event)
    {
        auto root1 = GetRootEntity(event.reg, event.a);
        auto root2 = GetRootEntity(event.reg, event.b);

        auto* move1 = event.reg->try_get<MovementComponent>(root1);
        auto* move2 = event.reg->try_get<MovementComponent>(root2);

        if (move1 && event.reg->get<ColliderComponent>(event.a).trigger)
            ChangeMovementMode(*event.reg, root1, *move1, Falling);

        if (move2 && event.reg->get<ColliderComponent>(event.b).trigger)
            ChangeMovementMode(*event.reg, root2, *move2, Falling);
    }

    void Jump(
            entt::registry& reg, entt::entity e,
            float jumpSpeed)
    {
        auto* move = reg.try_get<MovementComponent>(e);
        auto* vel = reg.try_get<VelocityComponent>(e);

        if (move && vel)
            Jump(reg, e, *move, *vel, jumpSpeed);
    }

    void Jump(
            entt::registry& reg, entt::entity e,
            MovementComponent& move, VelocityComponent& vel, float jumpSpeed)
    {
        // TMT_INFO << "Jump";

        vel.velocity.y = std::max(vel.velocity.y, 0.f) + jumpSpeed;

        ++move.jumpCnt;
        move.mode = Falling;
    }

    void Land(
            entt::registry& reg, entt::entity e,
            MovementComponent& move, VelocityComponent& vel)
    {
         TMT_INFO << "Land";

        vel.velocity.y = 0;

        ChangeMovementMode(reg, e, move, Walking);
    }

    void ChangeMovementMode(
            entt::registry& reg, entt::entity e,
            MovementComponent& move, MovementMode mode)
    {
        switch (mode)
        {
        case Falling:
            {
                if (--move.gndStayCnt == 0)
                {
                    move.mode = Falling;
//                    TMT_INFO << "Falling " << (int)e;
                }
            }
            break;
        case Walking:
            {
                ++move.gndStayCnt;
                move.mode = Walking;
                move.jumpCnt = 0;
//                TMT_INFO << "Walking " << (int)e;
            }
            break;
        }

        EventDispatcher::GetInstance().Enqueue(
                ChangeMovementModeEvent{
                    e, &reg, mode});
    }
}