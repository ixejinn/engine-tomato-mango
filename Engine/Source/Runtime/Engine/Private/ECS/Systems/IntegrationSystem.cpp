#include "ECS/Systems/IntegrationSystem.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Rigidbody.h"
#include "ECS/Components/Movement.h"
#include "ECS/SystemUpdateContexts.h"
#include "SimulationConfig.h"
#include "Utils/RegistryEntry.h"
REGISTER_SYSTEM(tomato::SystemPhase::Integration, IntegrationSystem)

namespace tomato {
    void IntegrationSystem::Update(SimContext& simCtx) {
        auto view = simCtx.registry.view<TransformComponent, VelocityComponent>();

        for (auto [e, trf, vel] : view.each()) {
            // TMT_INFO << vel.velocity.x << ", " << vel.velocity.y << ", " << vel.velocity.z;
            trf.AddPosition(vel.velocity * FIXED_DELTA_TIME);

            auto pos = trf.GetWorldPosition();
        }
    }
}