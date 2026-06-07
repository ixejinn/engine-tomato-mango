#include "ECS/Systems/GravitySystem.h"
#include "ECS/SystemUpdateContexts.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Rigidbody.h"
#include "Utils/RegistryEntry.h"
REGISTER_SYSTEM(tomato::SystemPhase::Physics, GravitySystem)

namespace tomato {
    void GravitySystem::Update(SimContext& simCtx) {
        auto view = simCtx.registry.view<MovementComponent, VelocityComponent>();

        for (auto [e, movement, velocity] : view.each()) {
            if (movement.mode == MovementMode::Walking)
                continue;

            velocity.velocity.y += GRAVITY;
        }
    }
}