#include "ECS/Systems/ParticleRenderSystem.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Camera.h"
#include "ECS/Components/Particle.h"
#include "ECS/SystemUpdateContexts.h"
#include "Resource/AssetHash.h"
#include "Resource/AssetRegistry.h"
#include "Resource/Render/Mesh.h"
#include "Resource/Render/Shader.h"
#include "Resource/Render/Texture.h"
#include "Simulation/SimulationConfig.h"
#include "Utils/RegistryEntry.h"
REGISTER_BUILT_IN_SYSTEM(tomato::SystemPhase::Particle, ParticleRenderSystem)

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
        if (!mesh2D_ || !shader_)
        {
            TMT_ERR << "Invalid mesh or shader.";
            return;
        }

        mesh2D_->Bind();
        shader_->Use();

        AssetRegistry<Texture>::GetInstance().Get(curTexture_)->Bind();

        auto& registry = simCtx.state->GetRegistry();

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

        auto view = registry.view<TransformComponent, ParticleEffectComponent>();
        for (auto [e, trf, particle] : view.each())
        {
            if (particle.activeCnt <= 0)
                continue;

            if (curTexture_ != particle.texture)
            {
                curTexture_ = particle.texture;
                AssetRegistry<Texture>::GetInstance().Get(curTexture_)->Bind();
            }

            for (int i = 0; i < particle.activeCnt; )
            {
                if (std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now() - particle.lifetimes[i].start
                        )
                        > particle.lifetimes[i].duration)
                {
                    int backIdx = particle.activeCnt - 1;
                    if (i == backIdx)
                    {
                        particle.activeCnt = 0;
                        break;
                    }

                    std::swap(particle.positions[i], particle.positions[backIdx]);
                    std::swap(particle.velocities[i], particle.velocities[backIdx]);
                    std::swap(particle.lifetimes[i], particle.lifetimes[backIdx]);
                    std::swap(particle.scales[i], particle.scales[backIdx]);

                    --particle.activeCnt;
                    continue;
                }

                particle.positions[i] += particle.velocities[i] * FIXED_DELTA_TIME;
                auto position = trf.GetWorldPosition() + particle.positions[i];

                auto T = glm::translate(glm::mat4(1.f), position);
                auto S = glm::scale(glm::mat4(1.f), glm::vec3(particle.scales[i]));
                shader_->SetUniformMat4("uModel", T * S);

                shader_->SetUniformInt("uTexture", 0);
                shader_->SetUniformVec4("uColor", particle.color);

                mesh2D_->Draw();
                ++i;
            }

            if (particle.activeCnt <= 0)
                simCtx.state->particlePool_.Release(e);
        }
    }
}