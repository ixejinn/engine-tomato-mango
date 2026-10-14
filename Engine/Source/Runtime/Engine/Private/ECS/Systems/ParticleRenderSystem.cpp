#include "ECS/Systems/ParticleRenderSystem.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"

#include "ECS/Components/Camera.h"
#include "ECS/Components/Visibility.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Particle.h"

#include "Render/GLStateCache.h"
#include "Resource/AssetHash.h"
#include "Resource/AssetRegistry.h"

#include "Resource/Render/Mesh.h"
#include "Resource/Render/Shader.h"
#include "Resource/Render/Texture.h"
#include "Resource/Render/ParticleEffect.h"

#include "Particle/ParticleEmitterPool.h"

#include "Simulation/SimulationConfig.h"
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
        auto& registry = simCtx.state->GetRegistry();
        auto& renderCtx = registry.ctx().get<RenderContext>();

        const auto* camera = registry.try_get<CameraComponent>(renderCtx.mainCam);
        if (!camera)
        {
            TMT_WARN << "Main camera is missing or invalid.";
            return;
        }

        // 활성화된 파티클 엔티티가 없으면 종료
        if (registry.ctx().get<ParticleEmitterPool>().GetActiveEntityCount() == 0)
            return;

        if (!mesh2D_ || !shader_)
        {
            TMT_ERR << "Invalid mesh or shader.";
            return;
        }

        auto& gl = renderCtx.glState;
        gl.Apply(PipelinePresets::Opaque);

        Texture* texture = AssetRegistry<Texture>::GetInstance().Get(curTexture_);

        gl.UseShader(shader_->GetHandle());
        gl.BindVertexArray(mesh2D_->GetHandle());
        gl.BindTexture(texture->GetHandle());

        // 카메라 축 설정
        auto& viewProjMat = camera->viewProjMat;

        shader_->SetUniformMat4("uViewProj", viewProjMat);
        shader_->SetUniformVec3(
         "uCamRight",
         glm::normalize(glm::vec3(viewProjMat[0][0], viewProjMat[1][0], viewProjMat[2][0])));
        shader_->SetUniformVec3(
         "uCamUp",
         glm::normalize(glm::vec3(viewProjMat[0][1], viewProjMat[1][1], viewProjMat[2][1])));

        // 컴포넌트 업데이트
        auto view = registry.view
            <TransformComponent, VisibilityComponent,
            ParticleEmitterComponent, ParticleRuntimeComponent,
            ParticleBufferComponent, ParticleRenderComponent>();
        for (auto [e, trf, visibiliy, emitter, runtime, buffer, render] : view.each())
        {
            if (!runtime.active)
                continue;

            if (!visibiliy.visible)
                continue;

            // 파티클 그리기
            texture = AssetRegistry<Texture>::GetInstance().Get(render.texture);
            gl.BindTexture(texture->GetHandle());

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
}