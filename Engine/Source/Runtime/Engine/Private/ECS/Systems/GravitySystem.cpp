#include "ECS/Systems/GravitySystem.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Rigidbody.h"
#include "Utils/Logger.h"

namespace tomato
{
    void GravitySystem::Update(SimContext& simCtx)
    {
        auto view = simCtx.state->GetRegistry().view<MovementComponent, VelocityComponent>();
        for (auto [e, movement, velocity] : view.each())
        {
            if (movement.mode == MovementMode::Walking)
                continue;

            velocity.velocity.y += GRAVITY;
            // TMT_INFO << (int)e << " add gravity: " << velocity.velocity.y;
        }
    }
}