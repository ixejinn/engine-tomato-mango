#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ECS/Systems/RenderSystem.h"
#include "ECS/Entity/Entity.h"
#include "ECS/Components/Camera.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Render.h"
#include "ECS/Components/Visibility.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "Resource/AssetHash.h"
#include "Resource/AssetRegistry.h"
#include "Resource/Render/Mesh.h"
#include "Resource/Render/Shader.h"
#include "Resource/Render/Texture.h"
#include "Services/Window.h"

namespace tomato
{
    RenderSystem::RenderSystem()
    : curMesh_(GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Cube)))
    , curShader_(GetAssetID(Shader::PrimitiveName))
    , curTexture_(GetAssetID(Texture::PrimitiveName))
    {
        // Enable depth test
        glEnable(GL_DEPTH_TEST);

        // Enable color blending and set blend function for alpha transparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        glEnable(GL_CULL_FACE);

        AssetRegistry<Mesh>::GetInstance().CreatePrimitives();
        AssetRegistry<Texture>::GetInstance().CreatePrimitives();
        AssetRegistry<Shader>::GetInstance().CreatePrimitives();
    }

    void RenderSystem::Update(SimContext& simCtx)
    {
        auto& registry = simCtx.state->GetRegistry();

        auto& renderCtx = registry.ctx().get<RenderContext>();
        const entt::entity mainCam = renderCtx.mainCam;
        const entt::entity skybox = renderCtx.skybox;
        const entt::entity viewGizmo = renderCtx.viewGizmo;

        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Get main camera from render context
        if (mainCam == entt::null)
        {
            TMT_WARN << "Main camera is not found.";
            return;
        }
        auto* mainCamComp = registry.try_get<CameraComponent>(mainCam);

        Mesh* mesh = AssetRegistry<Mesh>::GetInstance().Get(curMesh_);
        mesh->Bind();
        UpdateCullface(mesh->GetCullface());

        Shader* shader = AssetRegistry<Shader>::GetInstance().Get(curShader_);
        shader->Use();

        AssetRegistry<Texture>::GetInstance().Get(curTexture_)->Bind();

        auto group = registry.group<TransformComponent, RenderComponent>();
        for (auto [e, trf, render] : group.each()) {
            // TODO: frustum culling

            if (!IsVisible(registry, e))
                continue;

            if (render.shader == 0) render.shader = curShader_;
            if (curShader_ != render.shader)
            {
                curShader_ = render.shader;
                shader = AssetRegistry<Shader>::GetInstance().Get(curShader_);
                shader->Use();
            }

            if (render.texture == 0) render.texture = curTexture_;
            if (curTexture_ != render.texture)
            {
                curTexture_ = render.texture;
                AssetRegistry<Texture>::GetInstance().Get(curTexture_)->Bind();
            }

            if (render.mesh == 0) render.mesh = curMesh_;
            if (curMesh_ != render.mesh)
            {
                curMesh_ = render.mesh;
                mesh = AssetRegistry<Mesh>::GetInstance().Get(curMesh_);
                mesh->Bind();
                UpdateCullface(mesh->GetCullface());
            }

            const auto& mtx = trf.GetTransformMatrix();
            shader->SetUniformMat4("uModel", mtx);
            shader->SetUniformMat4("uViewProj", mainCamComp == nullptr ? glm::mat4(1.f) : mainCamComp->viewProjMat);
            shader->SetUniformMat3("uNormal", glm::transpose(glm::inverse(glm::mat3(mtx))));

            shader->SetUniformInt("uTexture", 0);
            shader->SetUniformVec3("uLightPos", glm::vec3(0, 10, 0));
            shader->SetUniformVec4("uColor", render.color);

            if (registry.all_of<RootEntityTag>(e))
                mesh->Draw();
            else
                mesh->Draw(true);
        }

        if (skybox != entt::null)
        {
            if (!IsVisible(registry, skybox))
                return;

            glCullFace(GL_FRONT);
            glDepthFunc(GL_LEQUAL);

            Shader* skyShader = AssetRegistry<Shader>::GetInstance().Get(GetAssetID("SkyboxShader"));
            skyShader->Use();

            Texture* skyTexture = AssetRegistry<Texture>::GetInstance().Get(GetAssetID("PrimitiveSkybox"));
            skyTexture->Bind();

            Mesh* skyMesh = AssetRegistry<Mesh>::GetInstance().Get(GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Cube)));
            skyMesh->Bind();

            skyShader->SetUniformMat4("uModel", glm::mat4(1.f));
            auto viewMtx = glm::mat4(glm::mat3(mainCamComp == nullptr ? glm::mat4(1.f) : mainCamComp->view));
            skyShader->SetUniformMat4("uViewProj", mainCamComp->projection * viewMtx);
            skyShader->SetUniformInt("uCubemap", 0);

            skyMesh->Draw();

            glCullFace(GL_BACK);
            glDepthFunc(GL_LESS);
        }

        if (viewGizmo != entt::null)
        {
            glViewport(-80, -80, 300, 300);
            glClear(GL_DEPTH_BUFFER_BIT);

            glm::vec3 viewGizmoLight =
                    registry.get<TransformComponent>(mainCam).GetWorldQuaternion() * glm::vec3(0, 0, 1);

            // Render view gizmo center
            auto& viewGizmoTrfMtx = registry.get<TransformComponent>(viewGizmo).GetTransformMatrix();
            auto& viewGizmoRender = registry.get<RenderComponent>(viewGizmo);

            curShader_ = viewGizmoRender.shader;
            shader = AssetRegistry<Shader>::GetInstance().Get(curShader_);
            shader->Use();

            curTexture_ = viewGizmoRender.texture;
            AssetRegistry<Texture>::GetInstance().Get(curTexture_)->Bind();

            curMesh_ = viewGizmoRender.mesh;
            mesh = AssetRegistry<Mesh>::GetInstance().Get(curMesh_);
            mesh->Bind();
            UpdateCullface(mesh->GetCullface());

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
            mesh->Bind();

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

            glViewport(0, 0, Window::GetWidth(), Window::GetHeight());
        }
    }

    void RenderSystem::UpdateCullface(bool meshCullface)
    {
        if (cullface_ ^ meshCullface)
        {
            if (meshCullface)
                glEnable(GL_CULL_FACE);
            else
                glDisable(GL_CULL_FACE);

            cullface_ = meshCullface;
        }
    }
}
