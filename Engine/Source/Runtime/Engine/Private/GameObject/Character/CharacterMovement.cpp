#include "CharacterMovement.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/Rigidbody.h"
#include "ECS/Entity/Hierarchy.h"
#include "Utils/Logger.h"

namespace tomato::CharacterMovement
{
    void ChangeMovementMode(ChangeMovementModeEvent& event)
    {
        switch (event.mode)
        {
        case Falling:
            {
                auto& move = event.reg->get<MovementComponent>(GetRootEntity(event.reg, event.e));
                if (--move.gndStayCnt == 0)
                {
                    move.mode = Falling;
                    TMT_INFO << "Falling " << (int)event.e;
                }
            }
            break;
        case Walking:
            {
                entt::entity root = GetRootEntity(event.reg, event.e);
                auto& move = event.reg->get<MovementComponent>(root);
                ++move.gndStayCnt;

                move.mode = Walking;
                move.jumpCnt = 0;

                event.reg->get<VelocityComponent>(root).velocity.y = 0;
                TMT_INFO << "Walking " << (int)event.e;
            }
            break;
        }
    }
}