#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ECS/Systems/RenderSystem.h"
#include "ECS/Entity/Entity.h"
#include "ECS/Components/Camera.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Render.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/ActiveTag.h"
#include "ECS/Components/Visibility.h"
#include "ECS/Components/Gizmo.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "Resource/AssetHash.h"
#include "Resource/AssetRegistry.h"
#include "Resource/Render/Mesh.h"
#include "Resource/Render/Shader.h"
#include "Resource/Render/Texture.h"
#include "Render/SortKey.h"
#include "Render/RenderPass.h"
#include "Profiler/CPUProfiler.h"
#include <ECS/Components/Nametag.h>
#include <Render/GLStateCache.h>

namespace tomato
{
    RenderSystem::RenderSystem()
    {
        AssetRegistry<Mesh>::GetInstance().CreatePrimitives();
        AssetRegistry<Texture>::GetInstance().CreatePrimitives();
        AssetRegistry<Shader>::GetInstance().CreatePrimitives();

        glClearColor(0.f, 0.f, 0.f, 1.0f);
    }

    void RenderSystem::Update(SimContext& simCtx)
    {
//        CPU_PROFILER_BLOCK_BEGIN(RenderSystem::Update);
        UpdateDrawList(simCtx);

        auto& registry = simCtx.state->GetRegistry();
        auto& renderCtx = registry.ctx().get<RenderContext>();

        auto& gl = renderCtx.glState;
        gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        const auto* camera = registry.try_get<CameraComponent>(renderCtx.mainCam);
        if (!camera)
        {
            TMT_WARN << "Main camera is missing or invalid.";
            return;
        }

        gl.Apply(PipelinePresets::Opaque);

        auto& shaderRegistry = AssetRegistry<Shader>::GetInstance();
        auto& texRegistry = AssetRegistry<Texture>::GetInstance();
        auto& meshRegistry = AssetRegistry<Mesh>::GetInstance();

        for (const auto& drawItem : drawList_)
        {
            auto& render = registry.get<RenderComponent>(drawItem.entity);
            Shader* shader = shaderRegistry.Get(render.shader);
            Mesh* mesh = meshRegistry.Get(render.mesh);
            Texture* texture = texRegistry.Get(render.texture);

            switch (GetRenderPass(drawItem.sortKey))
            {
                case RenderPass::Opaque:
                default:
                {
                    PipelineState state{PipelinePresets::Opaque};
                    if (render.doubleSided)
                        state.raster.cullEnabled = false;

                    if (auto* stencil = registry.try_get<StencilComponent>(drawItem.entity))
                    {
                        if (stencil->write != 0)
                        {

                        }
                    }

                    auto& trf = registry.get<TransformComponent>(drawItem.entity);

                    if (gl.UseShader(shader->GetHandle()))
                    {
                        shader->SetUniformMat4("uViewProj", camera->viewProjMat);
                        shader->SetUniformVec3("uLightPos", glm::vec3(0, 10, 0));
                        shader->SetUniformInt("uTexture", 0);
                    }

                    const auto& mtx = trf.GetTransformMatrix();
                    shader->SetUniformMat4("uModel", mtx);
                    shader->SetUniformMat3("uNormal", glm::transpose(glm::inverse(glm::mat3(mtx))));
                    shader->SetUniformVec4("uColor", render.color);

                    gl.BindTexture(texture->GetHandle());
                    gl.BindVertexArray(mesh->GetHandle());

                    gl.Apply(state);
#ifdef TOMATO_DEBUG
                    if (!registry.all_of<RootEntityTag>(drawItem.entity))
                        mesh->Draw(true);
                    else
#endif
                        mesh->Draw();
                }
                    break;

                case RenderPass::Skybox:
                {
                    gl.UseShader(shader->GetHandle());
                    glm::mat4 viewProj = camera->projection * glm::mat4(glm::mat3(camera->view));
                    shader->SetUniformMat4("uViewProj", viewProj);
                    shader->SetUniformMat4("uModel", glm::mat4(1.f));
                    shader->SetUniformInt("uCubemap", 0);

                    gl.BindTexture(texture->GetHandle());
                    gl.BindVertexArray(mesh->GetHandle());

                    gl.Apply(PipelinePresets::Skybox);
                    mesh->Draw();
                }
                    break;

                case RenderPass::Transparent:
                {
                    auto& trf = registry.get<TransformComponent>(drawItem.entity);

                    if (gl.UseShader(shader->GetHandle()))
                    {
                        shader->SetUniformMat4("uViewProj", camera->viewProjMat);
                        shader->SetUniformVec3("uLightPos", glm::vec3(0, 10, 0));
                    }

                    const auto& mtx = trf.GetTransformMatrix();
                    shader->SetUniformMat4("uModel", mtx);
                    shader->SetUniformMat3("uNormal", glm::transpose(glm::inverse(glm::mat3(mtx))));
                    shader->SetUniformInt("uTexture", 0);
                    shader->SetUniformVec4("uColor", render.color);

                    gl.BindTexture(texture->GetHandle());
                    gl.BindVertexArray(mesh->GetHandle());

                    gl.Apply(PipelinePresets::Transparent);
                    mesh->Draw();
                }
                    break;
            }
        }
//        CPU_PROFILER_BLOCK_END(RenderSystem::Update);
    }

    void RenderSystem::UpdateDrawList(SimContext& simCtx)
    {
        drawList_.clear();

        auto& registry = simCtx.state->GetRegistry();
        const entt::entity mainCam = registry.ctx().get<RenderContext>().mainCam;
        const auto& posCam = registry.get<TransformComponent>(mainCam).GetWorldPosition();

        auto& shaderRegistry = AssetRegistry<Shader>::GetInstance();
        auto& texRegistry = AssetRegistry<Texture>::GetInstance();
        auto& meshRegistry = AssetRegistry<Mesh>::GetInstance();

        auto view = registry.view<ActiveTag, VisibilityComponent, TransformComponent, RenderComponent>(entt::exclude<GizmoTag>);
        for (const auto& [e, visibility, trf, render] : view.each())
        {
            if (!IsVisible(visibility))
                continue;

            Shader* shader = shaderRegistry.Get(render.shader);
            Mesh* mesh = meshRegistry.Get(render.mesh);
            Texture* texture = texRegistry.Get(render.texture);

            if (!shader || !mesh || !texture)
            {
                TMT_WARN << "Invalid or missing resource detected (Shader / Texture / Mesh).";
                continue;
            }

            // TODO: frustum culling

            if (render.color.a < 1.f)
                render.priority = RenderPriority::Transparent;

            if (render.priority == RenderPriority::Transparent)
                drawList_.emplace_back(
                        GetTransparentSortKey(
                                render.priority,
                                glm::length2(trf.GetWorldPosition() - posCam),
                                shader->GetSortIndex()),
                        e);
            else
                drawList_.emplace_back(
                        GetOpaqueSortKey(
                                render.priority,
                                shader->GetSortIndex(),
                                texture->GetSortIndex(),
                                mesh->GetSortIndex()),
                        e);
        }

        std::ranges::sort(drawList_, std::ranges::less{}, &DrawItem::sortKey);
    }
}
