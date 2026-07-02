#include "ECS/Systems/IntegrationSystem.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Rigidbody.h"
#include "ECS/Components/Movement.h"
#include "ECS/SystemUpdateContexts.h"
#include "Simulation/SimulationConfig.h"
#include "Utils/RegistryEntry.h"
REGISTER_BUILT_IN_SYSTEM(tomato::SystemPhase::Integration, IntegrationSystem)

namespace tomato
{
    void IntegrationSystem::Update(SimContext& simCtx)
    {
        auto view = simCtx.state->GetRegistry().view<TransformComponent, VelocityComponent>();
        for (auto [e, trf, vel] : view.each())
        {
            // TMT_INFO << (int)e << " velocity: " << vel.velocity.x << ", " << vel.velocity.y << ", " << vel.velocity.z;
            trf.AddPosition(vel.velocity * FIXED_DELTA_TIME);

             // auto pos = trf.GetLocalPosition();
             // if (pos.y < 0) {
             //     trf.SetPosition(pos.x, 0, pos.z);
             //     auto& move = simCtx.state->GetRegistry().get<MovementComponent>(e);
             //     move.mode = Walking;
             //     move.jumpCnt = 0;
             //     vel.velocity.y = 0;
             //     std::cout << "walking\n";
             // }

            // pos = trf.GetLocalPosition();
            // TMT_INFO << (int)e << " position: " << pos.x << ", " << pos.y << ", " << pos.z;
        }
    }
}