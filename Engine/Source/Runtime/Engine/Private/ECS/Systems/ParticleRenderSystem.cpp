#include "ECS/Systems/ParticleRenderSystem.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Camera.h"
#include "ECS/Components/Particle.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "Resource/AssetHash.h"
#include "Resource/AssetRegistry.h"
#include "Resource/Render/Mesh.h"
#include "Resource/Render/Shader.h"
#include "Resource/Render/Texture.h"
#include "Resource/Render/ParticleEffect.h"
#include "Simulation/SimulationConfig.h"
#include "Utils/RandomNumberGenerator.h"

using namespace std::chrono_literals;

namespace tomato
{
    ParticleRenderSystem::ParticleRenderSystem()
    : curTexture_(GetAssetID(Texture::PrimitiveName))
    {
        mesh2D_ = AssetRegistry<Mesh>::GetInstance().Get(
            GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Plain)));
        shader_ = AssetRegistry<Shader>::GetInstance().Get(
            GetAssetID("ParticleShader"));
    }

    void ParticleRenderSystem::Update(SimContext& simCtx)
    {
        // 활성화된 파티클 엔티티가 없으면 종료
        if (simCtx.state->particlePool_.GetActiveEmitterNum() == 0)
            return;

        if (!mesh2D_ || !shader_)
        {
            TMT_ERR << "Invalid mesh or shader.";
            return;
        }
        mesh2D_->Bind();
        shader_->Use();
        AssetRegistry<Texture>::GetInstance().Get(curTexture_)->Bind();

        auto& registry = simCtx.state->GetRegistry();

        // 카메라 축 설정
        auto& [mainCam] = registry.ctx().get<RenderContext>();
        if (mainCam == entt::null)
            return;
        auto& viewProjMat = registry.try_get<CameraComponent>(mainCam)->viewProjMat;

        shader_->SetUniformMat4("uViewProj", viewProjMat);
        shader_->SetUniformVec3(
         "uCamRight",
         glm::normalize(glm::vec3(viewProjMat[0][0], viewProjMat[1][0], viewProjMat[2][0])));
        shader_->SetUniformVec3(
         "uCamUp",
         glm::normalize(glm::vec3(viewProjMat[0][1], viewProjMat[1][1], viewProjMat[2][1])));

        // 컴포넌트 업데이트
        auto view = registry.view<TransformComponent,
            ParticleEmitterComponent, ParticleRuntimeComponent,
            ParticleBufferComponent, ParticleRenderComponent>();
        for (auto [e, trf, emitter, runtime, buffer, render] : view.each())
        {
            if (!runtime.active)
                continue;

            // 이미터 lifetime 확인
            auto now = std::chrono::steady_clock::now();
            auto activeDuration =
                    std::chrono::duration_cast<std::chrono::milliseconds>(now - emitter.emitter.start);
            if (activeDuration >= emitter.emitter.duration)
            {
                if (emitter.looping)
                {
                    // std::cout << "   LOOPING(" << simCtx.tick << ") ----------\n";
                    emitter.emitter.start = now;

                    if (emitter.burst.has_value())
                    {
                        emitter.burst->adder = std::chrono::milliseconds::zero();
                        emitter.burst->latest = now;
                        emitter.burst->finishedCycles = 0;
                    }
                }
                else if (runtime.activeCnt == 0) // 루프 아닌데 활성화된 파티클이 없으면 풀에 반납(완전 종료)
                {
                    simCtx.state->particlePool_.Release(e);
                    continue;
                }
            }

            // lifetime 지난 파티클 제거
            for (int i = 0; i < runtime.activeCnt; )
            {
                if (std::chrono::duration_cast<std::chrono::milliseconds>(now - buffer.lifetimes[i].start)
                    >= buffer.lifetimes[i].duration)
                {
                    int backIdx = runtime.activeCnt - 1;
                    if (i == backIdx)
                    {
                        runtime.activeCnt = 0;
                        break;
                    }

                    std::swap(buffer.positions[i], buffer.positions[backIdx]);
                    std::swap(buffer.velocities[i], buffer.velocities[backIdx]);
                    std::swap(buffer.lifetimes[i], buffer.lifetimes[backIdx]);

                    --runtime.activeCnt;
                }
                else
                    ++i;
            }

            ParticleData pData{
                .emitter = emitter,
                .runtime = runtime,
                .buffer = buffer,
                .render = render
            };

            // rate over time 의한 파티클 생성
            if (emitter.emitPeriod > 0ms)
            {
                runtime.adder += std::chrono::duration_cast<std::chrono::milliseconds>(now - runtime.latestTP);
                runtime.latestTP = now;

                if (runtime.adder >= emitter.emitPeriod)
                {
                    runtime.adder -= emitter.emitPeriod;

                    if (emitter.space == World)
                        InitializeParticles(pData, trf);
                    else
                        InitializeParticles(pData);
                }
            }

            // burst 의한 파티클 생성
            if (emitter.burst.has_value())
            {
                emitter.burst->adder += std::chrono::duration_cast<std::chrono::milliseconds>(now - emitter.burst->latest);
                emitter.burst->latest = now;

                if (emitter.burst->finishedCycles < emitter.burst->cycles && emitter.burst->adder >= emitter.burst->period)
                {
                    ++emitter.burst->finishedCycles;

                    emitter.burst->adder -= emitter.burst->period;
                    if (emitter.space == World)
                        InitializeParticles(pData, trf, emitter.burst->count);
                    else
                        InitializeParticles(pData, emitter.burst->count);
                    // std::cout << "   BURST(" << simCtx.tick << ") ---------- " << pc.activeCnt << "\n";
                }
            }

            // 파티클 그리기
            if (curTexture_ != render.texture)
            {
                curTexture_ = render.texture;
                AssetRegistry<Texture>::GetInstance().Get(curTexture_)->Bind();
            }

            for (int i = 0; i < runtime.activeCnt; )
            {
                buffer.positions[i] += buffer.velocities[i] * FIXED_DELTA_TIME;
                auto position = trf.GetLocalQuaternion() * buffer.positions[i];
                if (emitter.space == Local)
                    position += trf.GetWorldPosition();

                auto T = glm::translate(glm::mat4(1.f), position);
                auto S = glm::scale(glm::mat4(1.f), glm::vec3(render.size));
                shader_->SetUniformMat4("uModel", T * S);

                shader_->SetUniformInt("uTexture", 0);
                shader_->SetUniformVec4("uColor", render.color);

                mesh2D_->Draw();
                ++i;
            }
        }
    }

    void ParticleRenderSystem::InitializeParticles(ParticleData& comp, int num)
    {
        int initCnt = std::min(comp.runtime.activeCnt + num, MAX_PARTICLE_NUM);

        auto now = std::chrono::steady_clock::now();
        for (int i = comp.runtime.activeCnt; i < initCnt; ++i)
        {
            comp.buffer.positions[i] = {0, 0, 0};
            comp.buffer.lifetimes[i] = {comp.emitter.particleLifetime, now};
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
                            {glm::cos(phiR) * glm::cos(lambdaR),
                             glm::sin(phiR),
                             glm::cos(phiR) * glm::sin(lambdaR)};
                    comp.buffer.velocities[i] *= comp.emitter.startSpeed;
                }
                break;

            case ParticleEffectShape::Circle:
                for (int i = comp.runtime.activeCnt; i < initCnt; ++i)
                {
                    int lambda = RandomNumberGenerator::GetUniformIntDistribution(0, 359);
                    auto lambdaR = glm::radians(static_cast<float>(lambda));

                    comp.buffer.velocities[i] = {glm::cos(lambdaR), 0, glm::sin(lambdaR)};
                    comp.buffer.velocities[i] *= comp.emitter.startSpeed;
                }
                break;

            case ParticleEffectShape::Cone:
                for (int i = comp.runtime.activeCnt; i < initCnt; ++i)
                {
                    int lambda = RandomNumberGenerator::GetUniformIntDistribution(0, 359);
                    float phi = RandomNumberGenerator::GetUniformRealDistribution(0.f, comp.emitter.angle);

                    auto lambdaR = glm::radians(static_cast<float>(lambda));
                    auto phiR = glm::radians(phi);

                    comp.buffer.velocities[i] =
                            {glm::cos(phiR) * glm::cos(lambdaR),
                             glm::sin(phiR),
                             glm::cos(phiR) * glm::sin(lambdaR)};
                    comp.buffer.velocities[i] *= comp.emitter.startSpeed;
                }
                break;
        }

        comp.runtime.activeCnt = initCnt;
    }

    void ParticleRenderSystem::InitializeParticles(ParticleData& comp, TransformComponent& trf, int num)
    {
        int initCnt = std::min(comp.runtime.activeCnt + num, MAX_PARTICLE_NUM);

        auto now = std::chrono::steady_clock::now();
        for (int i = comp.runtime.activeCnt; i < initCnt; ++i)
        {
            comp.buffer.positions[i] = trf.GetWorldPosition();
            comp.buffer.lifetimes[i] = {comp.emitter.particleLifetime, now};
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
                            {glm::cos(phiR) * glm::cos(lambdaR),
                             glm::sin(phiR),
                             glm::cos(phiR) * glm::sin(lambdaR)};
                    comp.buffer.velocities[i] *= comp.emitter.startSpeed;
                }
                break;

            case ParticleEffectShape::Circle:
                for (int i = comp.runtime.activeCnt; i < initCnt; ++i)
                {
                    int lambda = RandomNumberGenerator::GetUniformIntDistribution(0, 359);
                    auto lambdaR = glm::radians(static_cast<float>(lambda));

                    comp.buffer.velocities[i] = {glm::cos(lambdaR), 0, glm::sin(lambdaR)};
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

                    comp.buffer.velocities[i] =
                            {glm::cos(phiR) * glm::cos(lambdaR),
                             glm::sin(phiR),
                             glm::cos(phiR) * glm::sin(lambdaR)};
                    comp.buffer.velocities[i] *= comp.emitter.startSpeed;
                }
                break;
        }

        comp.runtime.activeCnt = initCnt;
    }
}