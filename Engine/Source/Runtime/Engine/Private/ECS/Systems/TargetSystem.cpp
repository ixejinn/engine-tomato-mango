#include "ECS/Systems/TargetSystem.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "ECS/Components/ActiveTag.h"
#include "ECS/Components/Target.h"
#include "ECS/Components/Transform.h"
#include "Simulation/SimulationConfig.h"

namespace tomato
{
    void TargetSystem::Update(SimContext& simCtx)
    {
        auto& registry = simCtx.state->GetRegistry();
        auto view = registry.view<ActiveTag, TransformComponent, TargetComponent>();
        for (auto [e, trf, target] : view.each())
        {
            if (!registry.valid(target.target))
                continue;

            auto& targetTrf = registry.get<TransformComponent>(target.target);

            glm::vec3 velocity = targetTrf.GetWorldPosition() - trf.GetWorldPosition();
            float velLen2 = glm::length2(velocity);
            if (velLen2 > 0)
            {
                velocity /= glm::sqrt(velLen2);
                velocity *= target.speed;

                trf.AddPosition(velocity * FIXED_DELTA_TIME);
            }
        }
    }
}
