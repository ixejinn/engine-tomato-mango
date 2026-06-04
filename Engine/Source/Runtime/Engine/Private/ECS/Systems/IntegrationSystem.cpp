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
            trf.AddPosition(vel.velocity * FIXED_DELTA_TIME);

            auto pos = trf.GetWorldPosition();
            // !!!!! TEMP JUMP !!!!!
            if (pos.y < 0) {
                trf.SetPosition(pos.x, 0.f, pos.z);
                vel.velocity.y = 0;

                if (auto move = simCtx.registry.try_get<MovementComponent>(e))
                    move->jumpCnt = 0;
            }
        }
    }
}