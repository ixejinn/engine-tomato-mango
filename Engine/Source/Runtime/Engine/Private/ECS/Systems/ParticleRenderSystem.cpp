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
        auto view = registry.view<TransformComponent, ParticleComponent>();
        for (auto [e, trf, pc] : view.each())
        {
            if (!pc.active)
                continue;

            // 이미터 lifetime 확인
            auto now = std::chrono::steady_clock::now();
            auto activeDuration =
                    std::chrono::duration_cast<std::chrono::milliseconds>(now - pc.emitter.start);
            if (activeDuration >= pc.emitter.duration)
            {
                if (pc.looping)
                {
                    // std::cout << "   LOOPING(" << simCtx.tick << ") ----------\n";
                    pc.emitter.start = now;

                    if (pc.burst.has_value())
                    {
                        pc.burst->adder = std::chrono::milliseconds::zero();
                        pc.burst->latest = now;
                        pc.burst->finishedCycles = 0;
                    }
                }
                else if (pc.activeCnt == 0) // 루프 아닌데 활성화된 파티클이 없으면 풀에 반납(완전 종료)
                {
                    simCtx.state->particlePool_.Release(e);
                    continue;
                }
            }

            // lifetime 지난 파티클 제거
            for (int i = 0; i < pc.activeCnt; )
            {
                if (std::chrono::duration_cast<std::chrono::milliseconds>(now - pc.lifetimes[i].start)
                    >= pc.lifetimes[i].duration)
                {
                    int backIdx = pc.activeCnt - 1;
                    if (i == backIdx)
                    {
                        pc.activeCnt = 0;
                        break;
                    }

                    std::swap(pc.positions[i], pc.positions[backIdx]);
                    std::swap(pc.velocities[i], pc.velocities[backIdx]);
                    std::swap(pc.lifetimes[i], pc.lifetimes[backIdx]);

                    --pc.activeCnt;
                }
                else
                    ++i;
            }

            // rate over time 의한 파티클 생성
            if (pc.emitPeriod > 0ms)
            {
                pc.adder += std::chrono::duration_cast<std::chrono::milliseconds>(now - pc.latestTP);
                pc.latestTP = now;

                if (pc.adder >= pc.emitPeriod)
                {
                    pc.adder -= pc.emitPeriod;

                    if (pc.space == World)
                        InitializeParticles(pc, trf);
                    else
                        InitializeParticles(pc);
                }
            }

            // burst 의한 파티클 생성
            if (pc.burst.has_value())
            {
                pc.burst->adder += std::chrono::duration_cast<std::chrono::milliseconds>(now - pc.burst->latest);
                pc.burst->latest = now;

                if (pc.burst->finishedCycles < pc.burst->cycles && pc.burst->adder >= pc.burst->period)
                {
                    ++pc.burst->finishedCycles;

                    pc.burst->adder -= pc.burst->period;
                    if (pc.space == World)
                        InitializeParticles(pc, trf, pc.burst->count);
                    else
                        InitializeParticles(pc, pc.burst->count);
                    // std::cout << "   BURST(" << simCtx.tick << ") ---------- " << pc.activeCnt << "\n";
                }
            }

            // 파티클 그리기
            if (curTexture_ != pc.texture)
            {
                curTexture_ = pc.texture;
                AssetRegistry<Texture>::GetInstance().Get(curTexture_)->Bind();
            }

            for (int i = 0; i < pc.activeCnt; )
            {
                pc.positions[i] += pc.velocities[i] * FIXED_DELTA_TIME;
                auto position = trf.GetLocalQuaternion() * pc.positions[i];
                if (pc.space == Local)
                    position += trf.GetWorldPosition();

                auto T = glm::translate(glm::mat4(1.f), position);
                auto S = glm::scale(glm::mat4(1.f), glm::vec3(pc.size));
                shader_->SetUniformMat4("uModel", T * S);

                shader_->SetUniformInt("uTexture", 0);
                shader_->SetUniformVec4("uColor", pc.color);

                mesh2D_->Draw();
                ++i;
            }
        }
    }

    void ParticleRenderSystem::InitializeParticles(ParticleComponent& comp, int num)
    {
        int initCnt = std::min(comp.activeCnt + num, ParticleComponent::MAX_PARTICLE);

        auto now = std::chrono::steady_clock::now();
        for (int i = comp.activeCnt; i < initCnt; ++i)
        {
            comp.positions[i] = {0, 0, 0};
            comp.lifetimes[i] = {comp.lifetime, now};
        }

        switch (comp.shape)
        {
            case ParticleEffectShape::Sphere:
                for (int i = comp.activeCnt; i < initCnt; ++i)
                {
                    int lambda = RandomNumberGenerator::GetUniformIntDistribution(0, 359);
                    int phi = RandomNumberGenerator::GetUniformIntDistribution(-90, 90);

                    auto lambdaR = glm::radians(static_cast<float>(lambda));
                    auto phiR = glm::radians(static_cast<float>(phi));

                    comp.velocities[i] =
                            {glm::cos(phiR) * glm::cos(lambdaR),
                             glm::sin(phiR),
                             glm::cos(phiR) * glm::sin(lambdaR)};
                    comp.velocities[i] *= comp.startSpeed;
                }
                break;

            case ParticleEffectShape::Circle:
                for (int i = comp.activeCnt; i < initCnt; ++i)
                {
                    int lambda = RandomNumberGenerator::GetUniformIntDistribution(0, 359);
                    auto lambdaR = glm::radians(static_cast<float>(lambda));

                    comp.velocities[i] = {glm::cos(lambdaR), 0, glm::sin(lambdaR)};
                    comp.velocities[i] *= comp.startSpeed;
                }
                break;

            case ParticleEffectShape::Cone:
                for (int i = comp.activeCnt; i < initCnt; ++i)
                {
                    int lambda = RandomNumberGenerator::GetUniformIntDistribution(0, 359);
                    float phi = RandomNumberGenerator::GetUniformRealDistribution(0.f, comp.angle);

                    auto lambdaR = glm::radians(static_cast<float>(lambda));
                    auto phiR = glm::radians(phi);

                    comp.velocities[i] =
                            {glm::cos(phiR) * glm::cos(lambdaR),
                             glm::sin(phiR),
                             glm::cos(phiR) * glm::sin(lambdaR)};
                    comp.velocities[i] *= comp.startSpeed;
                }
                break;
        }

        comp.activeCnt = initCnt;
    }

    void ParticleRenderSystem::InitializeParticles(ParticleComponent& comp, TransformComponent& trf, int num)
    {
        int initCnt = std::min(comp.activeCnt + num, ParticleComponent::MAX_PARTICLE);

        auto now = std::chrono::steady_clock::now();
        for (int i = comp.activeCnt; i < initCnt; ++i)
        {
            comp.positions[i] = trf.GetWorldPosition();
            comp.lifetimes[i] = {comp.lifetime, now};
        }

        switch (comp.shape)
        {
            case ParticleEffectShape::Sphere:
                for (int i = comp.activeCnt; i < initCnt; ++i)
                {
                    int lambda = RandomNumberGenerator::GetUniformIntDistribution(0, 359);
                    int phi = RandomNumberGenerator::GetUniformIntDistribution(-90, 90);

                    auto lambdaR = glm::radians(static_cast<float>(lambda));
                    auto phiR = glm::radians(static_cast<float>(phi));

                    comp.velocities[i] =
                            {glm::cos(phiR) * glm::cos(lambdaR),
                             glm::sin(phiR),
                             glm::cos(phiR) * glm::sin(lambdaR)};
                    comp.velocities[i] *= comp.startSpeed;
                }
                break;

            case ParticleEffectShape::Circle:
                for (int i = comp.activeCnt; i < initCnt; ++i)
                {
                    int lambda = RandomNumberGenerator::GetUniformIntDistribution(0, 359);
                    auto lambdaR = glm::radians(static_cast<float>(lambda));

                    comp.velocities[i] = {glm::cos(lambdaR), 0, glm::sin(lambdaR)};
                    comp.velocities[i] *= comp.startSpeed;
                }
                break;

            case ParticleEffectShape::Cone:
                for (int i = comp.activeCnt; i < initCnt; ++i)
                {
                    int lambda = RandomNumberGenerator::GetUniformIntDistribution(0, 359);
                    float phi = RandomNumberGenerator::GetUniformRealDistribution(0.f, comp.angle);

                    auto lambdaR = glm::radians(static_cast<float>(lambda));
                    auto phiR = glm::radians(90.f - phi);

                    comp.velocities[i] =
                            {glm::cos(phiR) * glm::cos(lambdaR),
                             glm::sin(phiR),
                             glm::cos(phiR) * glm::sin(lambdaR)};
                    comp.velocities[i] *= comp.startSpeed;
                }
                break;
        }

        comp.activeCnt = initCnt;
    }
}