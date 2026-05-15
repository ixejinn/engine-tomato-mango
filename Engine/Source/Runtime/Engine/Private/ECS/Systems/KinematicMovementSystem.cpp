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
            velocity.velocity.z = dir.y * move.horizontalSpeed;

            // Rotation (!!!!TEST!!!!)
            glm::vec3 eulerDegree = trf.GetEulerDegree();
            if (HasFlag(keypress, InputIntent::TurnLeft))
                trf.SetEulerDegree(eulerDegree + glm::vec3(0.f, 5.f, 0.f));
            if (HasFlag(keypress, InputIntent::TurnRight))
                trf.SetEulerDegree(eulerDegree + glm::vec3(0.f, -5.f, 0.f));
            if (HasFlag(keypress, InputIntent::TurnUp))
                trf.SetEulerDegree(eulerDegree + glm::vec3(5.f, 0.f, 0.f));
            if (HasFlag(keypress, InputIntent::TurnDown))
                trf.SetEulerDegree(eulerDegree + glm::vec3(-5.f, 0.f, 0.f));

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
