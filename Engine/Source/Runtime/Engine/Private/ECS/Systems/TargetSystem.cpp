#include <optional>
#include <entt/entt.hpp>
#include "ECS/Systems/TargetSystem.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "ECS/Components/ActiveTag.h"
#include "ECS/Components/Target.h"
#include "ECS/Components/Transform.h"
#include "Simulation/SimulationConfig.h"

namespace tomato
{
    namespace
    {
        std::optional<glm::vec3> GetGoalPosition(entt::registry& registry, const TargetComponent& target)
        {
            if (target.type == TargetType::Position)
                return target.targetPosition + target.offset;

            if (!registry.valid(target.targetEntity) || !registry.all_of<ActiveTag>(target.targetEntity))
                return std::nullopt;

            auto* trfTarget = registry.try_get<TransformComponent>(target.targetEntity);
            if (!trfTarget)
                return std::nullopt;

            return trfTarget->GetWorldPosition() + target.offset;
        }

        glm::vec3 GetChaseStep(TargetComponent& target, const glm::vec3& toGoal)
        {
            float distance = glm::length(toGoal);
            float remainingDist = distance - target.stopDistance;

            // Update target.state
            switch (target.state)
            {
            case TargetState::Idle:
                if (remainingDist > target.margin)
                {
                    target.state = TargetState::Waiting;
                    target.accumulator = 0.0f;
                }
                break;

            case TargetState::Waiting:
                target.accumulator += FIXED_DELTA_TIME;

                if (target.accumulator >= target.delay)
                    target.state = TargetState::Chasing;
                else if (remainingDist <= 0)
                    target.state = TargetState::Idle;
                break;

            case TargetState::Chasing:
                if (remainingDist <= 0)
                    target.state = TargetState::Idle;
                break;
            }

            // Calculate displacement
            glm::vec3 desiredVelocity{0};
            if (target.state == TargetState::Chasing)
            {
                float speed = std::min({
                    target.speed,                       // 최대 속력
                    remainingDist / FIXED_DELTA_TIME,   // remainingDist를 한 번에 지나는 속력
                    glm::sqrt(2.0f * target.acceleration * remainingDist)
                    // remainingDist 안에 target.acceleration의 등감속(최대 감속)으로 멈출 수 있는 한도 내에서 최대 속력
                    /** 설명 추가
                     * 속력 후보를 v라고 할 때 등감속하여 멈추려면 0 = v - at
                     * 등감속 환경에서 거리 = 평균 속력 * 시간, 평균 속력 = (v + 0) / 2
                     * d = (v / 2) * (v / a) = v^2 / 2a
                     * v = sqrt(2ad) (a: target.acceleration, d: remainingDist)
                     */
                });
                desiredVelocity = (toGoal / distance) * speed;
            }

            glm::vec3 deltaV = desiredVelocity - target.velocity;
            float maxDeltaV = target.acceleration * FIXED_DELTA_TIME;
            float deltaVLen2 = glm::length2(deltaV);
            if (deltaVLen2 > maxDeltaV * maxDeltaV)
                deltaV *= maxDeltaV / glm::sqrt(deltaVLen2);

            target.velocity += deltaV;
            return target.velocity * FIXED_DELTA_TIME;
        }
    }

    void TargetSystem::Update(SimContext& simCtx)
    {
        auto& registry = simCtx.state->GetRegistry();
        auto view = registry.view<ActiveTag, TransformComponent, TargetComponent>();
        for (auto [e, trf, target] : view.each())
        {
            auto goal = GetGoalPosition(registry, target);
            glm::vec3 direction = goal ? goal.value() - trf.GetWorldPosition() : glm::vec3{0};

            glm::vec3 displacement = (target.motion == TargetMotion::Lock) ? direction : GetChaseStep(target, direction);
            if (glm::length2(displacement) > 0)
                trf.AddPosition(displacement);
        }
    }
}
