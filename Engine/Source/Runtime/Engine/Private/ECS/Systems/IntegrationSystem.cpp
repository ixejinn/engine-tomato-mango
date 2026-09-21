#include "ECS/Systems/IntegrationSystem.h"
#include "ECS/Components/ActiveTag.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Rigidbody.h"
#include "ECS/Components/Movement.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "Simulation/SimulationConfig.h"

namespace tomato
{
    void IntegrationSystem::Update(SimContext& simCtx)
    {
        auto view = simCtx.state->GetRegistry().view<ActiveTag, TransformComponent, VelocityComponent>();
        for (auto [e, trf, vel] : view.each())
        {
            trf.AddPosition(vel.velocity * FIXED_DELTA_TIME);
        }
    }
}