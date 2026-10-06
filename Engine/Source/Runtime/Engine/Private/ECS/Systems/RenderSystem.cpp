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
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "Resource/AssetHash.h"
#include "Resource/AssetRegistry.h"
#include "Resource/Render/Mesh.h"
#include "Resource/Render/Shader.h"
#include "Resource/Render/Texture.h"
#include "Render/SortKey.h"
#include "Render/RenderPass.h"
#include "Services/Window.h"
#include "Profiler/CPUProfiler.h"

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

        auto* camera = registry.try_get<CameraComponent>(renderCtx.mainCam);
        if (renderCtx.mainCam == entt::null || !camera)
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

                    gl.Apply(PipelinePresets::Opaque);
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

//        if (viewGizmo != entt::null)
//        {
//            gl.SetViewport(-80, -80, 300, 300);
//            gl.Clear(GL_DEPTH_BUFFER_BIT);
//
//            glm::vec3 viewGizmoLight =
//                    registry.get<TransformComponent>(mainCam).GetWorldQuaternion() * glm::vec3(0, 0, 1);
//
//            // Render view gizmo center
//            auto& viewGizmoTrfMtx = registry.get<TransformComponent>(viewGizmo).GetTransformMatrix();
//            auto& viewGizmoRender = registry.get<RenderComponent>(viewGizmo);
//
//            curShader = viewGizmoRender.shader;
//            shader = AssetRegistry<Shader>::GetInstance().Get(curShader);
//            gl.UseShader(shader->GetHandle());
//
//            curTexture = viewGizmoRender.texture;
//            texture = AssetRegistry<Texture>::GetInstance().Get(curTexture);
//            gl.BindTexture(texture->GetHandle());
//
//            curMesh = viewGizmoRender.mesh;
//            mesh = AssetRegistry<Mesh>::GetInstance().Get(curMesh);
//            gl.BindVertexArray(mesh->GetHandle());
//
//            shader->SetUniformMat4("uModel", viewGizmoTrfMtx);
//            shader->SetUniformMat4("uViewProj",
//                glm::ortho(-1.5f, 1.5f, -1.5f, 1.5f, -1.5f, 1.5f)
//                * glm::mat4(glm::mat3(mainCamComp == nullptr ? glm::mat4(1.f) : mainCamComp->view)));
//            shader->SetUniformMat3("uNormal", glm::transpose(glm::inverse(glm::mat3(viewGizmoTrfMtx))));
//
//            shader->SetUniformInt("uTexture", 0);
//            shader->SetUniformVec3("uLightPos", viewGizmoLight);
//            shader->SetUniformVec4("uColor", viewGizmoRender.color);
//
//            mesh->Draw();
//
//            // Render view gizmo axis
//            mesh = AssetRegistry<Mesh>::GetInstance().Get(GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Cone)));
//            gl.BindVertexArray(mesh->GetHandle());
//
//            auto& gizmoAxes = registry.get<HierarchyComponent>(viewGizmo).children;
//            for (const entt::entity axis : gizmoAxes)
//            {
//                auto& axisTrfMtx = registry.get<TransformComponent>(axis).GetTransformMatrix();
//                auto& axisRender = registry.get<RenderComponent>(axis);
//
//                shader->SetUniformMat4("uModel", axisTrfMtx);
//                shader->SetUniformMat4("uViewProj",
//                    glm::ortho(-1.5f, 1.5f, -1.5f, 1.5f, -1.5f, 1.5f)
//                    * glm::mat4(glm::mat3(mainCamComp == nullptr ? glm::mat4(1.f) : mainCamComp->view)));
//                shader->SetUniformMat3("uNormal", glm::transpose(glm::inverse(glm::mat3(axisTrfMtx))));
//
//                shader->SetUniformInt("uTexture", 0);
//                shader->SetUniformVec3("uLightPos", viewGizmoLight);
//                shader->SetUniformVec4("uColor", axisRender.color);
//
//                mesh->Draw();
//            }
//
//            gl.SetViewport(0, 0, Window::GetWidth(), Window::GetHeight());
//        }

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

        auto view = registry.view<ActiveTag, VisibilityComponent, TransformComponent, RenderComponent>();
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
