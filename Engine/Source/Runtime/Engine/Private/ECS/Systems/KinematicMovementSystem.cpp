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
            if (HasFlag(keypress, ch.is1P ? InputIntent::Up : InputIntent::Up2))
                y++;
            if (HasFlag(keypress, ch.is1P ? InputIntent::Down : InputIntent::Down2))
                y--;
            if (HasFlag(keypress, ch.is1P ? InputIntent::Left : InputIntent::Left2))
                x--;
            if (HasFlag(keypress, ch.is1P ? InputIntent::Right : InputIntent::Right2))
                x++;
            glm::vec2 dir = glm::vec2{x, -y};
            if (glm::length(dir) > 1)
                dir = glm::normalize(dir);

            velocity.velocity.x = dir.x * move.horizontalSpeed;
            velocity.velocity.z = dir.y * move.horizontalSpeed;

            // Rotation (!!!!TEST!!!!)
            x = 0; y = 0;
            if (HasFlag(keypress, InputIntent::TurnLeft))
                x += 5;
            if (HasFlag(keypress, InputIntent::TurnRight))
                x -= 5;
            if (HasFlag(keypress, InputIntent::TurnUp))
                y += 5;
            if (HasFlag(keypress, InputIntent::TurnDown))
                y -= 5;
            glm::vec3 newDegree = trf.GetLocalEulerDegree() + glm::vec3{x, y, 0.f};
            trf.SetEulerDegree(newDegree);

            // Jump
            if (HasFlag(keydown, ch.is1P ? InputIntent::Jump : InputIntent::Jump2) && move.jumpCnt < JUMP_COUNT_MAX)
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
