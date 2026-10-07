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
#include "Services/Window.h"
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
    }

    void RenderSystem::Update(SimContext& simCtx)
    {
        CPU_PROFILER_BLOCK_BEGIN(RenderSystem::Update);
        UpdateDrawList(simCtx);

        auto& registry = simCtx.state->GetRegistry();

        auto& renderCtx = registry.ctx().get<RenderContext>();
        const entt::entity mainCam = renderCtx.mainCam;
        const entt::entity skybox = renderCtx.skybox;
        const entt::entity viewGizmo = renderCtx.viewGizmo;

        auto& gl = renderCtx.glState;
        glClearColor(0.f, 0.f, 0.f, 1.0f);
        gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        gl.Apply(PipelinePresets::Default3D);

        // Get main camera from render context
        if (mainCam == entt::null)
        {
            TMT_WARN << "Main camera is not found.";
            return;
        }
        auto* mainCamComp = registry.try_get<CameraComponent>(mainCam);

        AssetID curMesh = 0, curShader = 0, curTexture = 0;
        Mesh* mesh = nullptr; Shader* shader = nullptr; Texture* texture = nullptr;

        auto group = registry.group<TransformComponent, RenderComponent>();
        for (auto [e, trf, render] : group.each()) {
            // TODO: frustum culling

            if (!IsVisible(registry, e))
                continue;

            PipelineState state = PipelinePresets::Default3D;
            if (registry.get<NametagComponent>(e).name == "Ground")
            {
                //state.depth.testEnabled = false;
                state.stencil.enabled = true;
                state.stencil.ref = 1;
                state.stencil.dppass = GL_REPLACE;
            }

            if (render.shader == 0)
                render.shader = GetAssetID(Shader::PrimitiveName);
                
            if (curShader != render.shader)
            {
                curShader = render.shader;
                shader = AssetRegistry<Shader>::GetInstance().Get(curShader);
                gl.UseShader(shader->GetHandle());
            }

            if (render.texture == 0)
                render.texture = GetAssetID(Texture::PrimitiveName);
            if (curTexture != render.texture)
            {
                curTexture = render.texture;
                texture = AssetRegistry<Texture>::GetInstance().Get(curTexture);
                gl.BindTexture(texture->GetHandle());
            }
            
            if (render.mesh == 0)
                render.mesh = GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Cube));

            if (curMesh != render.mesh)
            {
                curMesh = render.mesh;
                //PipelineState state = PipelinePresets::Default3D;

                if (curMesh == GetAssetID("Primitive::OpenCylinder_50_10"))
                {
                    //glDisable(GL_CULL_FACE);
                    state.raster.cullEnabled = false;

                    state.stencil.enabled = true;
                    state.stencil.func = GL_EQUAL;
                    state.stencil.ref = 1;
                    state.stencil.dppass = GL_REPLACE;
                    state.stencil.writeMask = 0x00;
                    //std::cout << "OepnCylinder\n";
                }
                //gl.Apply(state);

                mesh = AssetRegistry<Mesh>::GetInstance().Get(curMesh);
                gl.BindVertexArray(mesh->GetHandle());
            }

            //if (curMesh == GetAssetID("Primitive::OpenCylinder_50_10"))
            //{
            //    //glDisable(GL_CULL_FACE);
            //    state.raster.cullEnabled = false;
            //    state.stencil.enabled = true;
            //    state.stencil.func = GL_EQUAL;
            //    state.stencil.ref = 1;
            //    state.stencil.dppass = GL_REPLACE;
            //    state.stencil.writeMask = 0x00;
            //    //std::cout << "OepnCylinder\n";
            //}

            const auto& mtx = trf.GetTransformMatrix();
            shader->SetUniformMat4("uModel", mtx);
            shader->SetUniformMat4("uViewProj", mainCamComp == nullptr ? glm::mat4(1.f) : mainCamComp->viewProjMat);
            shader->SetUniformMat3("uNormal", glm::transpose(glm::inverse(glm::mat3(mtx))));

            shader->SetUniformInt("uTexture", 0);
            shader->SetUniformVec3("uLightPos", glm::vec3(0, 10, 0));
            shader->SetUniformVec4("uColor", render.color);

            gl.Apply(state);

            if (registry.all_of<RootEntityTag>(e))
                mesh->Draw();
            else
                mesh->Draw(true);
        }

        if (skybox != entt::null)
        {
            if (IsVisible(registry, skybox))
            {
                gl.Apply(PipelinePresets::Skybox);

                Shader* skyShader = AssetRegistry<Shader>::GetInstance().Get(GetAssetID("SkyboxShader"));
                gl.UseShader(skyShader->GetHandle());

                Texture* skyTexture = AssetRegistry<Texture>::GetInstance().Get(GetAssetID("PrimitiveSkybox"));
                gl.BindTexture(skyTexture->GetHandle());

                Mesh* skyMesh = AssetRegistry<Mesh>::GetInstance().Get(GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Cube)));
                gl.BindVertexArray(skyMesh->GetHandle());

                skyShader->SetUniformMat4("uModel", glm::mat4(1.f));
                glm::mat4 viewProj{ 1.f };
                if (mainCamComp != nullptr)
                    viewProj = mainCamComp->projection * glm::mat4(glm::mat3(mainCamComp->view));

                skyShader->SetUniformMat4("uViewProj", viewProj);/*
                auto viewMtx = glm::mat4(glm::mat3(mainCamComp == nullptr ? glm::mat4(1.f) : mainCamComp->view));
                skyShader->SetUniformMat4("uViewProj", mainCamComp->projection * viewMtx);*/
                skyShader->SetUniformInt("uCubemap", 0);

                skyMesh->Draw();

                gl.Apply(PipelinePresets::Default3D);
            }
        }

        if (viewGizmo != entt::null)
        {
            gl.SetViewport(-80, -80, 300, 300);
            gl.Clear(GL_DEPTH_BUFFER_BIT);

            glm::vec3 viewGizmoLight =
                    registry.get<TransformComponent>(mainCam).GetWorldQuaternion() * glm::vec3(0, 0, 1);

            // Render view gizmo center
            auto& viewGizmoTrfMtx = registry.get<TransformComponent>(viewGizmo).GetTransformMatrix();
            auto& viewGizmoRender = registry.get<RenderComponent>(viewGizmo);

            curShader = viewGizmoRender.shader;
            shader = AssetRegistry<Shader>::GetInstance().Get(curShader);
            gl.UseShader(shader->GetHandle());

            curTexture = viewGizmoRender.texture;
            texture = AssetRegistry<Texture>::GetInstance().Get(curTexture);
            gl.BindTexture(texture->GetHandle());

            curMesh = viewGizmoRender.mesh;
            mesh = AssetRegistry<Mesh>::GetInstance().Get(curMesh);
            gl.BindVertexArray(mesh->GetHandle());

            shader->SetUniformMat4("uModel", viewGizmoTrfMtx);
            shader->SetUniformMat4("uViewProj",
                glm::ortho(-1.5f, 1.5f, -1.5f, 1.5f, -1.5f, 1.5f)
                * glm::mat4(glm::mat3(mainCamComp == nullptr ? glm::mat4(1.f) : mainCamComp->view)));
            shader->SetUniformMat3("uNormal", glm::transpose(glm::inverse(glm::mat3(viewGizmoTrfMtx))));

            shader->SetUniformInt("uTexture", 0);
            shader->SetUniformVec3("uLightPos", viewGizmoLight);
            shader->SetUniformVec4("uColor", viewGizmoRender.color);

            mesh->Draw();

            // Render view gizmo axis
            mesh = AssetRegistry<Mesh>::GetInstance().Get(GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Cone)));
            gl.BindVertexArray(mesh->GetHandle());

            auto& gizmoAxes = registry.get<HierarchyComponent>(viewGizmo).children;
            for (const entt::entity axis : gizmoAxes)
            {
                auto& axisTrfMtx = registry.get<TransformComponent>(axis).GetTransformMatrix();
                auto& axisRender = registry.get<RenderComponent>(axis);

                shader->SetUniformMat4("uModel", axisTrfMtx);
                shader->SetUniformMat4("uViewProj",
                    glm::ortho(-1.5f, 1.5f, -1.5f, 1.5f, -1.5f, 1.5f)
                    * glm::mat4(glm::mat3(mainCamComp == nullptr ? glm::mat4(1.f) : mainCamComp->view)));
                shader->SetUniformMat3("uNormal", glm::transpose(glm::inverse(glm::mat3(axisTrfMtx))));

                shader->SetUniformInt("uTexture", 0);
                shader->SetUniformVec3("uLightPos", viewGizmoLight);
                shader->SetUniformVec4("uColor", axisRender.color);

                mesh->Draw();
            }

            gl.SetViewport(0, 0, Window::GetWidth(), Window::GetHeight());
        }

        CPU_PROFILER_BLOCK_END(RenderSystem::Update);
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

            if (render.priority == RenderPriority::Transparent)
                drawList_.emplace_back(GetTransparentSortKey(
                    render.priority,
                    glm::length2(trf.GetWorldPosition() - posCam),
                    shaderRegistry.Get(render.shader)->GetSortIndex()), e);
            else
                drawList_.emplace_back(GetOpaqueSortKey(
                    render.priority,
                    shaderRegistry.Get(render.shader)->GetSortIndex(),
                    texRegistry.Get(render.texture)->GetSortIndex(),
                    meshRegistry.Get(render.mesh)->GetSortIndex()), e);
        }

        std::ranges::sort(drawList_, std::ranges::less{}, &DrawItem::sortKey);
    }
}
