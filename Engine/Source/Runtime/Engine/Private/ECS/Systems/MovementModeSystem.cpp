#include "ECS/Systems/MovementModeSystem.h"
#include "ECS/Components/OnCollision.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Rigidbody.h"
#include "ECS/SystemUpdateContexts.h"
#include "State/State.h"
#include "Utils/Logger.h"
#include "Utils/RegistryEntry.h"
//REGISTER_SYSTEM(tomato::SystemPhase::OnTrigger, MovementModeSystem);

namespace tomato
{
    void MovementModeSystem::Update(SimContext& simCtx)
    {
        auto& registry = simCtx.state->GetRegistry();
        auto view = registry.view<OnTriggerComponent, MovementComponent, VelocityComponent>();
        for (auto [e, trg, move, vel] : view.each())
        {
            if (trg.type == Exit)
            {
                StartFalling(move);
                TMT_INFO << "Falling " << (int)e;
            }
            else if (trg.type == Enter)
            {
                AfterLanding(move, vel);
                TMT_INFO << "Walking " << (int)e;
            }

            registry.remove<OnTriggerComponent>(e);
        }
    }

    void MovementModeSystem::StartFalling(MovementComponent& move)
    {
        if (--move.gndStayCnt == 0)
            move.mode = Falling;
    }

    void MovementModeSystem::AfterLanding(MovementComponent& move, VelocityComponent& vel)
    {
        ++move.gndStayCnt;

        move.mode = Walking;
        move.jumpCnt = 0;
        vel.velocity.y = 0;
    }
}