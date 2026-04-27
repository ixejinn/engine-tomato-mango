#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include "ECS/Systems/KinematicMovementSystem.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Rigidbody.h"
#include "ECS/Components/Movement.h"
#include "ECS/SystemUpdateContexts.h"
#include "SimulationConfig.h"
#include "Utils/BitmaskOperators.h"
#include "Utils/RegistryEntry.h"
REGISTER_SYSTEM(tomato::SystemPhase::Input, KinematicMovementSystem)

namespace tomato {
    void KinematicMovementSystem::Update(SimContext &simCtx) {
        auto input = simCtx.registry.ctx().get<InputContext*>();

        auto view = simCtx.registry.view<TransformComponent, SpeedComponent, InputChannelComponent, JumpComponent>();

        for (auto [e, trf, speed, ch, jump] : view.each()) {
            auto pos = trf.GetPosition();
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

            pos.x += dir.x * speed.speed * FIXED_DELTA_TIME;

            // !!! for 2D MOVEMENT !!!
            //pos.position.y += dir.y * speed.speed * Engine::FIXED_DELTA_TIME;

            // !!! for 3D MOVEMENT !!!
            pos.z += dir.y * speed.speed * FIXED_DELTA_TIME;

            // Jump
            if (HasFlag(keydown, InputIntent::Jump) && jump.cnt < JUMP_COUNT_MAX)
            {
                // Start jump
                jump.cnt++;
                jump.vy = std::max(jump.vy, 0.f) + JUMP_SPEED;
            }

            if (jump.cnt > 0)
            {
                // Jumping
                jump.vy += GRAVITY * FIXED_DELTA_TIME;
                pos.y += jump.vy * FIXED_DELTA_TIME;

                if (pos.y <= 0)
                {
                    // End jump
                    jump.cnt = 0;
                    pos.y = 0;
                    jump.vy = 0.f;
                }
            }

            trf.SetPosition(pos);
        }
    }
}
