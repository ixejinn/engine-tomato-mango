#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include "ECS/Systems/CharacterMovementSystem.h"
#include "ECS/Components/ActiveTag.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Velocity.h"
#include "ECS/Components/Movement.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "Utils/Bitmask/BitmaskOperators.h"
#include "GameObject/Character/CharacterMovement.h"
#include "GameObject/Character/CharacterMovementConfig.h"
#include "Simulation/SimulationConfig.h"

namespace tomato {
    void CharacterMovementSystem::Update(SimContext &simCtx)
    {
        auto& registry = simCtx.state->GetRegistry();

        auto view = registry.view<ActiveTag, VelocityComponent, InputChannelComponent, MovementComponent>();
        for (auto [e, velocity, ch, move] : view.each())
        {
            const auto& inputRec = simCtx.state->GetPlayerInputTimelines()[ch.channel][simCtx.tick];
            if (inputRec.tick != simCtx.tick)
                continue;
            InputIntent keypress{inputRec.held};
            InputIntent keydown{inputRec.down};

            // Move
            int x = 0, y = 0;
            if (HasFlag(keypress, ch.useWASD ? InputIntent::Up : InputIntent::Up2))
                ++y;
            if (HasFlag(keypress, ch.useWASD ? InputIntent::Down : InputIntent::Down2))
                --y;
            if (HasFlag(keypress, ch.useWASD ? InputIntent::Left : InputIntent::Left2))
                --x;
            if (HasFlag(keypress, ch.useWASD ? InputIntent::Right : InputIntent::Right2))
                ++x;

            glm::vec2 input{x, -y};
            if (x != 0 && y != 0)
                input = glm::normalize(input);

            switch (move.control)
            {
                case InputControl::Direct:
                    velocity.input = glm::vec3(input.x, 0, input.y) * velocity.horizontalSpeed;
                    break;
                case InputControl::Accelerate:
                {
                    velocity.input = glm::vec3(0.0f);

                    float curSpeed = glm::dot(glm::vec2(velocity.velocity.x, velocity.velocity.z), input);
                    float addSpeed = velocity.horizontalSpeed - curSpeed;
                    if (addSpeed > 0)
                        velocity.velocity += std::min(velocity.acceleration * FIXED_DELTA_TIME, addSpeed) * glm::vec3(input.x, 0, input.y);
                }
                    break;
            }

            // Rotation (!!!!TEST!!!!)
//            x = 0; y = 0;
//            if (HasFlag(keypress, InputIntent::TurnLeft))
//                y += 5;
//            if (HasFlag(keypress, InputIntent::TurnRight))
//                y -= 5;
////            if (HasFlag(keypress, InputIntent::TurnUp))
////                x += 5;
////            if (HasFlag(keypress, InputIntent::TurnDown))
////                x -= 5;
//            glm::vec3 newDegree = trf.GetLocalRotationDegree() + glm::vec3{x, y, 0.f};
//            trf.SetRotationDegree(newDegree);

            // Jump
            if (HasFlag(keydown, ch.useWASD ? InputIntent::Jump : InputIntent::Jump2) && move.jumpCnt < JUMP_COUNT_MAX)
                CharacterMovement::Jump(registry, e, move, velocity, JUMP_SPEED);
        }
    }
}
