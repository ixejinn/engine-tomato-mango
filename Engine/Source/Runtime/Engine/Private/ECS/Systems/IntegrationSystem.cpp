#include "ECS/Systems/IntegrationSystem.h"
#include "ECS/Components/ActiveTag.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Velocity.h"
#include "ECS/Components/Movement.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "Simulation/SimulationConfig.h"
#include "Collision/CollisionTolerance.h"

namespace tomato
{
    void IntegrationSystem::Update(SimContext& simCtx)
    {
        auto view = simCtx.state->GetRegistry().view<ActiveTag, TransformComponent, VelocityComponent>();
        for (auto [e, trf, vel] : view.each())
        {
            auto velocity = vel.Get();
            if (glm::length2(velocity) > VELOCITY_SQ_EPSILON)
            {
                std::cout << glm::length(velocity) << "\n";
                trf.AddPosition(velocity * FIXED_DELTA_TIME);
            }
        }
    }
}