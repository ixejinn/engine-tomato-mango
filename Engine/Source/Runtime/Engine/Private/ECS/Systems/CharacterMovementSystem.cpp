#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
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

        auto view = registry.view<ActiveTag, VelocityComponent, PlayerControlledComponent, MovementComponent>();
        for (auto [e, velocity, controlled, move] : view.each())
        {
            const auto& snapshot = simCtx.state->GetPlayerInputTimelines()[controlled.playerIdx][simCtx.tick];
            if (snapshot.tick != simCtx.tick)
                continue;
            const InputAction down{snapshot.down};
            const InputAction pressed{snapshot.pressed};

            // Move
            int x = 0, y = 0;
            if (HasFlag(down, controlled.useWASD ? InputAction::Up : InputAction::Up2))
                ++y;
            if (HasFlag(down, controlled.useWASD ? InputAction::Down : InputAction::Down2))
                --y;
            if (HasFlag(down, controlled.useWASD ? InputAction::Left : InputAction::Left2))
                --x;
            if (HasFlag(down, controlled.useWASD ? InputAction::Right : InputAction::Right2))
                ++x;

            glm::vec2 input{x, -y};
            if (x != 0 && y != 0)
                input = glm::normalize(input);

            velocity.input = glm::vec3(0);
            if (x != 0 || y != 0)
            {
                switch (move.control)
                {
                    case InputControl::Direct:
                        velocity.input = glm::vec3(input.x, 0, input.y) * velocity.horizontalSpeed;
                        break;
                    case InputControl::Accelerate:
                    {
                        velocity.input = glm::vec3(0.0f);

                        glm::vec2 horizontal{velocity.velocity.x, velocity.velocity.z};
                        float prevSpeed = glm::length(horizontal);
                        float speedAlongInput = glm::dot(horizontal, input);
                        float addSpeed = velocity.horizontalSpeed - speedAlongInput;

                        if (addSpeed > 0)
                        {
                            horizontal += std::min(velocity.acceleration * FIXED_DELTA_TIME, addSpeed) * input;

                            // 입력으로 속력이 최고 속도(velocity.horizontalSpeed) 이상으로 빨라지지 않도록 제한
                            // prevSpeed가 입력 이외 요소로 더 빨라진 경우는 그대로 유지
                            float limit = std::max(velocity.horizontalSpeed, prevSpeed);
                            float speed = glm::length(horizontal);
                            if (speed > limit)
                                horizontal *= limit / speed;

                            velocity.velocity.x = horizontal.x;
                            velocity.velocity.z = horizontal.y;
                        }
                    }
                        break;
                }
            }

            // Jump
            if (HasFlag(pressed, controlled.useWASD ? InputAction::Jump : InputAction::Jump2)
                && move.jumpCnt < JUMP_COUNT_MAX)
                CharacterMovement::Jump(registry, e, move, velocity, JUMP_SPEED);
        }
    }
}
