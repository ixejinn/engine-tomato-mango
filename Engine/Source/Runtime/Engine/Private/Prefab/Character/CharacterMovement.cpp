#include "Prefab/Character/CharacterMovement.h"
#include "Collision/CollisionEvent.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/Rigidbody.h"
#include "ECS/Entity/Hierarchy.h"
#include "Utils/Logger.h"

namespace tomato::CharacterMovement
{
    void StartFalling(const TriggerExitEvent& event)
    {
        ChangeMovementMode(Falling, event.reg, event.e1);
        ChangeMovementMode(Falling, event.reg, event.e2);
    }

    void AfterLanding(const TriggerEnterEvent& event) {
        ChangeMovementMode(Walking, event.reg, event.e1);
        ChangeMovementMode(Walking, event.reg, event.e2);
    }

    void ChangeMovementMode(MovementMode mode, entt::registry* reg, entt::entity e)
    {
        switch (mode)
        {
        case Falling:
            {
                if (auto* move = reg->try_get<MovementComponent>(GetRootEntity(reg, e)))
                {
                    if (--move->gndStayCnt == 0)
                    {
                        move->mode = Falling;
                        // TMT_INFO << "Falling " << (int)e;
                    }
                }
            }
            break;
        case Walking:
            {
                entt::entity root = GetRootEntity(reg, e);
                if (auto* move = reg->try_get<MovementComponent>(root))
                {
                    ++move->gndStayCnt;

                    move->mode = Walking;
                    move->jumpCnt = 0;

                    if (auto* vel = reg->try_get<VelocityComponent>(root))
                        vel->velocity.y = 0;
                    // TMT_INFO << "Walking " << (int)e;
                }
            }
            break;
        }
    }
}