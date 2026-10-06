#include "ECS/Systems/ViewGizmoRenderSystem.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "ECS/Components/Camera.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Render.h"
#include "ECS/Components/Gizmo.h"
#include "Resource/AssetRegistry.h"
#include "Resource/Render/Mesh.h"
#include "Resource/Render/Shader.h"
#include "Resource/Render/Texture.h"
#include "Services/Window.h"

namespace tomato
{
    void ViewGizmoRenderSystem::Update(SimContext& simCtx)
    {
        auto& registry = simCtx.state->GetRegistry();
        auto& renderCtx = registry.ctx().get<RenderContext>();
        auto& gl = renderCtx.glState;

        const auto* camera = registry.try_get<CameraComponent>(renderCtx.mainCam);
        if (!camera)
            return;

        gl.SetViewport(-80, -80, 300, 300);
        gl.Clear(GL_DEPTH_BUFFER_BIT);

        glm::vec3 light =
            registry.get<TransformComponent>(renderCtx.mainCam).GetWorldQuaternion() * glm::vec3(0, 0, 1);

        Shader* shader = AssetRegistry<Shader>::GetInstance().Get(GetAssetID(Shader::PrimitiveName));
        Texture* texture = AssetRegistry<Texture>::GetInstance().Get(GetAssetID(Texture::PrimitiveName));
        Mesh* mesh = nullptr;

        gl.UseShader(shader->GetHandle());
        shader->SetUniformMat4("uViewProj",
            glm::ortho(-1.5f, 1.5f, -1.5f, 1.5f, -1.5f, 1.5f)
            * glm::mat4(glm::mat3(camera->view)));
        shader->SetUniformVec3("uLightPos", light);
        shader->SetUniformInt("uTexture", 0);

        gl.BindTexture(texture->GetHandle());
        gl.Apply(PipelinePresets::Opaque);

        auto& meshRegistry = AssetRegistry<Mesh>::GetInstance();

        auto view = registry.view<TransformComponent, RenderComponent, GizmoTag>();
        for (const auto& [e, trf, render] : view.each())
        {
            mesh = meshRegistry.Get(render.mesh);
            gl.BindVertexArray(mesh->GetHandle());

            const auto& mtx = trf.GetTransformMatrix();
            shader->SetUniformMat4("uModel", mtx);
            shader->SetUniformMat3("uNormal", glm::transpose(glm::inverse(glm::mat3(mtx))));
            shader->SetUniformVec4("uColor", render.color);

            mesh->Draw();
        }

        gl.SetViewport(0, 0, Window::GetWidth(), Window::GetHeight());
    }
}
