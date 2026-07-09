#include "CharacterMovement.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/Rigidbody.h"
#include "ECS/Components/Transform.h"
#include "ECS/Entity/Hierarchy.h"
#include "Particle/ParticleEmitterPool.h"
#include "State/State.h"
#include "Utils/Logger.h"

namespace tomato::CharacterMovement
{
    void ChangeMovementMode(ChangeMovementModeEvent& event)
    {
        auto& registry = event.state->GetRegistry();
        switch (event.mode)
        {
        case Falling:
            {
                auto& move = registry.get<MovementComponent>(GetRootEntity(registry, event.e));
                if (--move.gndStayCnt == 0)
                {
                    move.mode = Falling;
                    TMT_INFO << "Falling " << (int)event.e;
                }
            }
            break;
        case Walking:
            {
                entt::entity root = GetRootEntity(registry, event.e);
                auto& move = registry.get<MovementComponent>(root);
                ++move.gndStayCnt;

                event.state->particlePool_.Acquire(Jump, registry.get<TransformComponent>(event.e).GetWorldPosition());

                move.mode = Walking;
                move.jumpCnt = 0;

                registry.get<VelocityComponent>(root).velocity.y = 0;
                TMT_INFO << "Walking " << (int)event.e;
            }
            break;
        }
    }
}