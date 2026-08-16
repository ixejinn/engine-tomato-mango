#include "ECS/Systems/ParticleEmissionSystem.h"

#include "ECS/SystemFramework/SystemUpdateContexts.h"

#include "ECS/Components/Transform.h"
#include "ECS/Components/Particle.h"
#include "ECS/Components/Target.h"

#include "Simulation/SimulationConfig.h"
#include "Utils/RandomNumberGenerator.h"

#include "Particle/ParticleEmitterPool.h"

#include "ECS/Entity/Entity.h"

using namespace std::chrono_literals;
namespace tomato
{
	void ParticleEmissionSystem::Update(SimContext& simCtx)
	{
        auto& registry = simCtx.state->GetRegistry();

		// 활성화된 파티클 엔티티가 없으면 종료
		if (registry.ctx().get<ParticleEmitterPool>().GetActiveEmitterNum() == 0)
			return;

		auto view = registry.view<ParticleEmitterComponent, ParticleRuntimeComponent,
			ParticleBufferComponent, ParticleRenderComponent>();
		for (auto [e, emitter, runtime, buffer, render] : view.each())
		{
            if (!runtime.active)
                continue;

            ParticleData pData{
                .emitter = emitter,
                .runtime = runtime,
                .buffer = buffer,
                .render = render
            };

            if (ProcessEmitterLifeTime(simCtx, e, pData))
                continue;

            ProcessParticleLifeTime(pData);

            RateOverTimeParticle(registry, e, pData);
            BurstParticle(registry, e, pData);
		}
	}

    void ParticleEmissionSystem::UpdateTransform(entt::registry& reg, entt::entity cur, const glm::quat& pQuat, const glm::vec3& pScale, const glm::mat4& pMatrix, bool pDirty)
    {
        auto rootView = reg.view<TransformComponent, ParticleRuntimeComponent, TargetComponent>();

        for (auto [e, trf, runtime, target] : rootView.each())
        {
            if (target.target == 0) continue;
            auto& targetTrf = reg.get<TransformComponent>(GetEntityByUUID(reg, target.target));

            auto T = glm::translate(glm::mat4(1.f), trf.GetLocalPosition());
            auto R = glm::toMat4(trf.GetLocalQuaternion());
            auto S = glm::scale(glm::mat4(1.f), trf.GetLocalScale());

        }
    }

    bool ParticleEmissionSystem::ProcessEmitterLifeTime(SimContext& simCtx, entt::entity e, ParticleData& particle)
    {
        // 이미터 lifetime 확인
        auto now = std::chrono::steady_clock::now();
        auto activeDuration =
            std::chrono::duration_cast<std::chrono::milliseconds>(now - particle.emitter.emitter.start);
        if (activeDuration > particle.emitter.emitter.duration)
        {
            if (particle.emitter.looping)
            {
                //std::cout << "   LOOPING(" << simCtx.tick << ") ----------\n";
                particle.emitter.emitter.start = now;

                if (particle.emitter.burst.has_value())
                {
                    particle.emitter.burst->adder = std::chrono::milliseconds::zero();
                    particle.emitter.burst->latest = now;
                    particle.emitter.burst->finishedCycles = 0;
                }
            }
            else if (particle.runtime.activeCnt == 0) // 루프 아닌데 활성화된 파티클이 없으면 풀에 반납(완전 종료)
            {
                simCtx.state->GetRegistry().ctx().get<ParticleEmitterPool>().Release(e);
                return true;
            }
        }

        return false;
    }

    void ParticleEmissionSystem::ProcessParticleLifeTime(ParticleData& particle)
    {
        // lifetime 지난 파티클 제거
        for (int i = 0; i < particle.runtime.activeCnt; )
        {
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(now - particle.buffer.lifetimes[i].start)
                >= particle.buffer.lifetimes[i].duration)
            {
                int backIdx = particle.runtime.activeCnt - 1;
                if (i == backIdx)
                {
                    particle.runtime.activeCnt = 0;
                    break;
                }

                std::swap(particle.buffer.positions[i],     particle.buffer.positions[backIdx]);
                std::swap(particle.buffer.velocities[i],    particle.buffer.velocities[backIdx]);
                std::swap(particle.buffer.lifetimes[i],     particle.buffer.lifetimes[backIdx]);

                --particle.runtime.activeCnt;
            }
            else
                ++i;
        }
    }
    void ParticleEmissionSystem::RateOverTimeParticle(entt::registry& reg, entt::entity e, ParticleData& particle)
    {
        // rate over time 의한 파티클 생성
        auto now = std::chrono::steady_clock::now();
        auto activeDuration =
            std::chrono::duration_cast<std::chrono::milliseconds>(now - particle.emitter.emitter.start);
        
        if (activeDuration <= particle.emitter.emitter.duration && particle.emitter.emitPeriod > 0ms)
        {
            auto now = std::chrono::steady_clock::now();
            particle.runtime.adder += std::chrono::duration_cast<std::chrono::milliseconds>(now - particle.runtime.latestTP);
            particle.runtime.latestTP = now;

            if (particle.runtime.adder >= particle.emitter.emitPeriod)
            {
                particle.runtime.adder -= particle.emitter.emitPeriod;

                if (particle.emitter.space == World)
                    InitializeParticles(particle, reg.try_get<TransformComponent>(e));
                else
                    InitializeParticles(particle);
            }
        }
    }

    void ParticleEmissionSystem::BurstParticle(entt::registry& reg, entt::entity e, ParticleData& particle)
    {
        if (particle.emitter.burst.has_value())
        {
            auto now = std::chrono::steady_clock::now();
            particle.emitter.burst->adder += std::chrono::duration_cast<std::chrono::milliseconds>(now - particle.emitter.burst->latest);
            particle.emitter.burst->latest = now;

            if (particle.emitter.burst->finishedCycles < particle.emitter.burst->cycles && particle.emitter.burst->adder >= particle.emitter.burst->period)
            {
                ++particle.emitter.burst->finishedCycles;

                particle.emitter.burst->adder -= particle.emitter.burst->period;
                if (particle.emitter.space == World)
                    InitializeParticles(particle, reg.try_get<TransformComponent>(e), particle.emitter.burst->count);
                else
                    InitializeParticles(particle, nullptr, particle.emitter.burst->count);
                // std::cout << "   BURST(" << simCtx.tick << ") ---------- " << pc.activeCnt << "\n";
            }
        }
    }

    void ParticleEmissionSystem::UpdateAliveParticles(ParticleData& particle)
    {
        for (int i = 0; i < particle.runtime.activeCnt; )
            particle.buffer.positions[i] += particle.buffer.velocities[i] * FIXED_DELTA_TIME;
    }

    void ParticleEmissionSystem::InitializeParticles(ParticleData& comp, TransformComponent* transform, int num)
    {
        int initCnt = std::min(comp.runtime.activeCnt + num, MAX_PARTICLE_NUM);

        auto now = std::chrono::steady_clock::now();
        for (int i = comp.runtime.activeCnt; i < initCnt; ++i)
        {
            comp.buffer.positions[i] =
                transform == nullptr ?
                glm::vec3{ 0, 0, 0 } : transform->GetWorldPosition();
            comp.buffer.lifetimes[i] = { comp.emitter.particleLifetime, now };
        }

        switch (comp.emitter.shape)
        {
        case ParticleEffectShape::Sphere:
            for (int i = comp.runtime.activeCnt; i < initCnt; ++i)
            {
                int lambda = RandomNumberGenerator::GetUniformIntDistribution(0, 359);
                int phi = RandomNumberGenerator::GetUniformIntDistribution(-90, 90);

                auto lambdaR = glm::radians(static_cast<float>(lambda));
                auto phiR = glm::radians(static_cast<float>(phi));

                comp.buffer.velocities[i] =
                { glm::cos(phiR) * glm::cos(lambdaR),
                 glm::sin(phiR),
                 glm::cos(phiR) * glm::sin(lambdaR) };
                comp.buffer.velocities[i] *= comp.emitter.startSpeed;
            }
            break;

        case ParticleEffectShape::Circle:
            for (int i = comp.runtime.activeCnt; i < initCnt; ++i)
            {
                int lambda = RandomNumberGenerator::GetUniformIntDistribution(0, 359);
                auto lambdaR = glm::radians(static_cast<float>(lambda));

                comp.buffer.velocities[i] = { glm::cos(lambdaR), 0, glm::sin(lambdaR) };
                comp.buffer.velocities[i] *= comp.emitter.startSpeed;
            }
            break;

        case ParticleEffectShape::Cone:
            for (int i = comp.runtime.activeCnt; i < initCnt; ++i)
            {
                int lambda = RandomNumberGenerator::GetUniformIntDistribution(0, 359);
                float phi = RandomNumberGenerator::GetUniformRealDistribution(0.f, comp.emitter.angle);

                auto lambdaR = glm::radians(static_cast<float>(lambda));
                auto phiR = glm::radians(90.f - phi);

                comp.buffer.velocities[i] = {
                    glm::cos(phiR) * glm::cos(lambdaR),
                    glm::sin(phiR),
                    glm::cos(phiR) * glm::sin(lambdaR)
                };
                comp.buffer.velocities[i] *= comp.emitter.startSpeed;
            }
            break;
        }

        comp.runtime.activeCnt = initCnt;
    }
}