#include <cmath>
#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include "ECS/Systems/FrictionSystem.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "ECS/Components/ActiveTag.h"
#include "ECS/Components/Velocity.h"
#include "ECS/Components/Movement.h"
#include "Simulation/SimulationConfig.h"

namespace tomato
{
    void FrictionSystem::Update(SimContext& simCtx)
    {
        auto view = simCtx.state->GetRegistry().view<ActiveTag, VelocityComponent, FrictionComponent, MovementComponent>();
        for (auto [e, velocity, friction, movement] : view.each())
        {
            switch (movement.mode)
            {
                case MovementMode::Walking:
                {
                    glm::vec2 horizontal{velocity.velocity.x, velocity.velocity.z};
                    float speed = glm::length(horizontal);
                    float drop = friction.groundFriction * FIXED_DELTA_TIME;
                    horizontal = (speed > drop) ? ((speed - drop) / speed) * horizontal : glm::vec2(0.0f);
                    velocity.velocity.x = horizontal.x;
                    velocity.velocity.z = horizontal.y;
                }
                    break;
                case MovementMode::Falling:
                {
                    float y = velocity.velocity.y;
                    velocity.velocity *= std::exp(-friction.airDrag * FIXED_DELTA_TIME);
                    velocity.velocity.y = y;
                }
                    break;
            }
        }
    }
}