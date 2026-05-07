#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include "ECS/Systems/KinematicMovementSystem.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Rigidbody.h"
#include "ECS/Components/Movement.h"
#include "ECS/SystemUpdateContexts.h"
#include "Utils/BitmaskOperators.h"
#include "Utils/RegistryEntry.h"
REGISTER_SYSTEM(tomato::SystemPhase::Input, KinematicMovementSystem)

namespace tomato {
    void KinematicMovementSystem::Update(SimContext &simCtx) {
        auto input = simCtx.registry.ctx().get<InputContext*>();

        auto view = simCtx.registry.view<TransformComponent, VelocityComponent, InputChannelComponent, MovementComponent>();

        for (auto [e, trf, velocity, ch, move] : view.each()) {
            const auto& inputRec = input->timelines[ch.channel][simCtx.tick];
            if (inputRec.tick != simCtx.tick)
                continue;

            InputIntent keypress{inputRec.held};
            InputIntent keydown{inputRec.down};

            // Move
            int x = 0, y = 0;
            if (HasFlag(keypress, InputIntent::Up))
                y++;
            if (HasFlag(keypress, InputIntent::Down))
                y--;
            if (HasFlag(keypress, InputIntent::Left))
                x--;
            if (HasFlag(keypress, InputIntent::Right))
                x++;
            glm::vec2 dir = glm::vec2{x, -y};
            if (glm::length(dir) > 1)
                dir = glm::normalize(dir);

            velocity.velocity.x = dir.x * move.horizontalSpeed;

            // !!! for 2D MOVEMENT !!!
            //pos.position.y += dir.y * speed.speed * Engine::FIXED_DELTA_TIME;

            // !!! for 3D MOVEMENT !!!
            velocity.velocity.z = dir.y * move.horizontalSpeed;

            // Jump
            if (HasFlag(keydown, InputIntent::Jump) && move.jumpCnt < JUMP_COUNT_MAX)
            {
                // Start move
                move.jumpCnt++;
                velocity.velocity.y = std::max(velocity.velocity.y, 0.f) + JUMP_SPEED;
            }

            if (move.jumpCnt > 0)
                velocity.velocity.y += GRAVITY;
        }
    }
}
