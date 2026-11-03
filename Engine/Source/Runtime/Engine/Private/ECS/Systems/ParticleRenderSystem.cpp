#include "ECS/Systems/ParticleRenderSystem.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "ECS/Components/Camera.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Visibility.h"
#include "ECS/Components/Particle.h"
#include "ECS/Entity/Entity.h"
#include "Resource/AssetRegistry.h"
#include "Resource/AssetHash.h"
#include "Resource/Render/Mesh.h"
#include "Resource/Render/Shader.h"
#include "Resource/Render/Texture.h"
#include "Particle/ParticleEmitterPool.h"
#include "Render/GLStateCache.h"

namespace tomato
{
    ParticleRenderSystem::ParticleRenderSystem()
        : texture_(AssetRegistry<Texture>::GetInstance().Get(GetAssetID(Texture::PrimitiveName)))
    {
        plain_ = AssetRegistry<Mesh>::GetInstance().Get(GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Plain)));
        shader_ = AssetRegistry<Shader>::GetInstance().Get(GetAssetID("ParticleShader"));
    }

    void ParticleRenderSystem::Update(SimContext& simCtx)
    {
        auto& registry = simCtx.state->GetRegistry();

        // 활성화된 이미터가 없으면 종료
        if (registry.ctx().get<ParticleEmitterPool>().GetActiveEntityCount() == 0)
            return;

        if (!plain_ || !shader_)
        {
            TMT_ERR << "Invalid mesh or shader.";
            return;
        }

        auto& renderCtx = registry.ctx().get<RenderContext>();
        const auto* camera = registry.try_get<CameraComponent>(renderCtx.mainCam);
        if (!camera)
            return;

        auto& gl = renderCtx.glState;
        gl.Apply(PipelinePresets::Opaque);
        gl.UseShader(shader_->GetHandle());
        gl.BindVertexArray(plain_->GetHandle());

        // 카메라 축 설정 (프레임 공통 유니폼)
        auto& viewProjMat = camera->viewProjMat;

        shader_->SetUniformMat4("uViewProj", viewProjMat);
        shader_->SetUniformVec3("uCamRight",
         glm::normalize(glm::vec3(viewProjMat[0][0], viewProjMat[1][0], viewProjMat[2][0])));
        shader_->SetUniformVec3("uCamUp",
         glm::normalize(glm::vec3(viewProjMat[0][1], viewProjMat[1][1], viewProjMat[2][1])));
        shader_->SetUniformInt("uTexture", 0);

        auto& texRegistry = AssetRegistry<Texture>::GetInstance();

        auto view = registry.view<ActiveTag, TransformComponent, VisibilityComponent,
        ParticleEmitterComponent, ParticleSimulationComponent>();
        for (auto [e, trf, visibility, emitter, sim] : view.each())
        {
            if (!IsVisible(visibility))
                continue;

            // 이미터 설정 (이미터 공통 유니폼)
            Texture* texture = emitter.sets.texture == 0 ? texture_ : texRegistry.Get(emitter.sets.texture);

            gl.BindTexture(texture->GetHandle());
            shader_->SetUniformVec4("uColor", emitter.sets.color);

            const glm::mat4 S = glm::scale(glm::mat4{1}, glm::vec3(emitter.sets.size));

            // 로컬 파티클은 월드로 이동
            glm::quat emitterRot = glm::quat{1, 0, 0, 0};
            glm::vec3 emitterPos = glm::vec3{0};
            if (emitter.sets.simSpace == ParticleSimulationSpace::Local)
            {
                emitterRot = trf.GetWorldQuaternion();
                emitterPos = trf.GetWorldPosition();
            }

            for (uint32_t i = 0; i < sim.activeCnt; ++i)
            {
                const glm::mat4 T = glm::translate(glm::mat4{1}, emitterRot * sim.positions[i] + emitterPos);
                shader_->SetUniformMat4("uModel", T * S);
                plain_->Draw();
            }
        }
    }
}