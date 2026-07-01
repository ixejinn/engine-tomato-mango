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
        entt::entity root1 = GetRootEntity(event.reg, event.e1);
        if (auto* move1 = event.reg->try_get<MovementComponent>(root1))
        {
            if (--move1->gndStayCnt == 0)
            {
                move1->mode = Falling;
                TMT_INFO << "Falling " << (int)event.e1;
            }
        }

        entt::entity root2 = GetRootEntity(event.reg, event.e2);
        if (auto* move2 = event.reg->try_get<MovementComponent>(root2))
        {
            if (--move2->gndStayCnt == 0)
            {
                move2->mode = Falling;
                TMT_INFO << "Falling " << (int)event.e2;
            }
        }
    }

    void AfterLanding(const TriggerEnterEvent& event) {
        entt::entity root1 = GetRootEntity(event.reg, event.e1);
        if (auto* move1 = event.reg->try_get<MovementComponent>(root1))
        {
            ++move1->gndStayCnt;

            move1->mode = Walking;
            move1->jumpCnt = 0;
            event.reg->get<VelocityComponent>(root1).velocity.y = 0;
            TMT_INFO << "Walking " << (int)event.e1 << " " << event.tick;
        }

        entt::entity root2 = GetRootEntity(event.reg, event.e2);
        if (auto* move2 = event.reg->try_get<MovementComponent>(root2))
        {
            ++move2->gndStayCnt;

            move2->mode = Walking;
            move2->jumpCnt = 0;
            event.reg->get<VelocityComponent>(root2).velocity.y = 0;
            TMT_INFO << "Walking " << (int)event.e2 << " " << event.tick;
        }
    }
}