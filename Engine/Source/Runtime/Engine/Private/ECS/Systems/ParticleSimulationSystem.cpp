#include <entt/entt.hpp>
#include "ECS/Systems/ParticleSimulationSystem.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "ECS/Components/ActiveTag.h"
#include "ECS/Components/Particle.h"
#include "ECS/Components/Transform.h"
#include "Simulation/SimulationConfig.h"
#include "Particle/ParticleType.h"
#include "Particle/ParticleEmitterPool.h"
#include "Particle/ParticleEmitterSettings.h"
#include "Utils/RandomNumberGenerator.h"

namespace tomato
{
    namespace
    {
        /**
         * @return ParticleBurst::time < time 까지 실행된 사이클 수
         */
        uint32_t CountBurstCyclesBefore(const ParticleBurst& burst, float time)
        {
            if (time <= burst.time)
                return 0;

            if (burst.interval <= 0)
                return burst.cycles;

            const auto passedCycles = static_cast<uint32_t>(std::ceil((time - burst.time) / burst.interval));
            return std::min(passedCycles, burst.cycles);
        }

        glm::vec3 RandomEmissionDirection(ParticleEmitterShape shape, float coneAngleDeg)
        {
            const float lambda = glm::radians(RandomNumberGenerator::GetUniformRealDistribution(0.0f, 360.0f));

            switch (shape)
            {
            case ParticleEmitterShape::Cone:
                {
                    const float minY = glm::cos(glm::radians(coneAngleDeg));
                    const float y = RandomNumberGenerator::GetUniformRealDistribution(minY, 1.0f);
                    const float r = glm::sqrt(1.0f - y * y);
                    return {r * glm::cos(lambda), y, r * glm::sin(lambda)};
                }
            case ParticleEmitterShape::Sphere:
                {
                    const float y = RandomNumberGenerator::GetUniformRealDistribution(-1.f, 1.f);   // 높이
                    const float r = glm::sqrt(1.0f - y * y);    // 높이가 y일때 구의 단면을 이루는 원의 반지름
                    return {r * glm::cos(lambda), y, r * glm::sin(lambda)};
                }
            case ParticleEmitterShape::Circle:
                return {glm::cos(lambda), 0, glm::sin(lambda)};
            }

            return glm::vec3{0, 1, 0};
        }
    }

    ParticleSimulationSystem::ParticleSimulationSystem()
    {
        finishedEmitters_.reserve(ParticleEmitterPoolTraits::N);
    }

    void ParticleSimulationSystem::Update(SimContext& simCtx)
    {
        auto& registry = simCtx.state->GetRegistry();
        auto& pool = registry.ctx().get<ParticleEmitterPool>();

        // 활성화된 이미터가 없으면 종료
        if (pool.GetActiveEntityCount() == 0)
            return;

        finishedEmitters_.clear();

        auto view = registry.view<ActiveTag, TransformComponent, ParticleEmitterComponent, ParticleSimulationComponent>();
        for (auto [e, trf, emitter, sim] : view.each())
        {
            UpdateParticles(sim);   // 파티클 먼저 갱신해야 새로 생성된 파티클이 생성 위치에서 시작
            UpdateEmitter(emitter.sets, sim, trf);

            if (!emitter.sets.looping && sim.time >= emitter.sets.duration && sim.activeCnt == 0)
                finishedEmitters_.push_back(e);
        }

        for (entt::entity e : finishedEmitters_)
            pool.Release(registry, e);
    }

    void ParticleSimulationSystem::UpdateParticles(ParticleSimulationComponent& sim)
    {
        for (uint32_t i = 0; i < sim.activeCnt; )
        {
            sim.remainingLifetimes[i] -= FIXED_DELTA_TIME;

            // 파티클 종료 swap-and-pop
            if (sim.remainingLifetimes[i] <= 0)
            {
                const uint32_t last = --sim.activeCnt;
                if (i != last)
                {
                    sim.positions[i]  = sim.positions[last];
                    sim.velocities[i] = sim.velocities[last];
                    sim.remainingLifetimes[i] = sim.remainingLifetimes[last];
                }

                continue;   // last에서 i로 옮겨진 원소는 갱신되지 않았으므로 i를 올리지 않음
            }

            sim.positions[i] += sim.velocities[i] * FIXED_DELTA_TIME;
            ++i;
        }
    }

    void ParticleSimulationSystem::UpdateEmitter(const ParticleEmitterSettings& sets, ParticleSimulationComponent& sim,
        const TransformComponent& trf)
    {
        const float prev = sim.time;
        float curr = prev + FIXED_DELTA_TIME;

        if (sets.duration <= 0) // 음수면 sets.startDelay 대기 중
        {
            sim.time = curr;
            return;
        }

        // 루프 재시작
        if (sets.looping && curr >= sets.duration)
        {
            EmitInWindow(sets, sim, trf, prev, sets.duration);  // [prev, sets.duration)
            curr = std::fmod(curr - sets.duration, sets.duration);
            EmitInWindow(sets, sim, trf, 0.0f, curr);           // [0, curr)
        }
        else
            EmitInWindow(sets, sim, trf, prev, std::min(curr, sets.duration));

        sim.time = curr;
    }

    void ParticleSimulationSystem::EmitInWindow(const ParticleEmitterSettings& sets, ParticleSimulationComponent& sim,
        const TransformComponent& trf, float from, float to)
    {
        from = std::max(from, 0.0f);
        if (to <= from)
            return;

        uint32_t count = 0; // 생성할 파티클 수

        //// Rate over time
        // sets.rateOverTime * 경과 시간 개의 파티클 생성
        // 소수점은 sets.accumulator에 누적해서 다음 틱으로 넘김
        if (sets.rateOverTime > 0)
        {
            sim.accumulator += sets.rateOverTime * (to - from);
            const auto cntROT = static_cast<uint32_t>(sim.accumulator);
            sim.accumulator -= static_cast<float>(cntROT);
            count += cntROT;
        }

        //// Burst
        for (const auto& burst : sets.bursts)
        {
            const auto cycles = CountBurstCyclesBefore(burst, to) - CountBurstCyclesBefore(burst, from);
            count += cycles * burst.count;
        }

        if (count > 0)
            Emit(sets, sim, trf, count);
    }

    void ParticleSimulationSystem::Emit(const ParticleEmitterSettings& sets, ParticleSimulationComponent& sim,
        const TransformComponent& trf, uint32_t count)
    {
        const uint32_t limit = std::min(sets.maxParticles, static_cast<uint32_t>(sim.positions.size()));
        const uint32_t begin = sim.activeCnt;
        const uint32_t end = std::min(begin + count, limit);
        if (begin >= end)
            return;

        glm::vec3 origin{0};
        auto rotation = glm::identity<glm::quat>();             // Local: 이미터 위치가 원점이 됨 (렌더 전 이미터 트랜스폼 적용)
        if (sets.simSpace == ParticleSimulationSpace::World)    // World: 생성 시점의 이미터 트랜스폼을 파티클에 적용 (그대로 렌더)
        {
            origin = trf.GetWorldPosition();
            rotation = trf.GetWorldQuaternion();
        }

        for (uint32_t i = begin; i < end; ++i)
        {
            sim.positions[i]  = origin;
            sim.velocities[i] = rotation * RandomEmissionDirection(sets.shape, sets.coneAngle) * sets.speed;
            sim.remainingLifetimes[i] = sets.lifetime;
        }

        sim.activeCnt = end;
    }
}
