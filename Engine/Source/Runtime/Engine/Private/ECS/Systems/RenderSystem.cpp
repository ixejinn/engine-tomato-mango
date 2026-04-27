#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ECS/Systems/RenderSystem.h"
#include "ECS/Components/Camera.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Render.h"
#include "ECS/SystemUpdateContexts.h"
#include "Resource/AssetHash.h"
#include "Resource/AssetRegistry.h"
#include "Resource/Render/Mesh.h"
#include "Resource/Render/Shader.h"
#include "Resource/Render/Texture.h"
#include "Utils/RegistryEntry.h"
//REGISTER_SYSTEM(tomato::SystemPhase::Rendering, RenderSystem);

namespace tomato {
    RenderSystem::RenderSystem()
    : curMesh_(GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Cube))),
    curShader_(GetAssetID(Shader::PrimitiveName)),
    curTexture_(GetAssetID(Texture::PrimitiveName)) {
        // Enable depth test
        glEnable(GL_DEPTH_TEST);

        // Enable color blending and set blend function for alpha transparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_CULL_FACE);

        AssetRegistry<Mesh>::GetInstance().CreatePrimitives();
        AssetRegistry<Texture>::GetInstance().CreatePrimitives();
        AssetRegistry<Shader>::GetInstance().CreatePrimitives();
    }

    void RenderSystem::Update(SimContext& simCtx) {
        auto& renderCtx = simCtx.registry.ctx().get<RenderContext>();

        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Get main camera from render context
        if (renderCtx.mainCam == entt::null) {
            TMT_WARN << "Main camera is not found.";
            return;
        }
        auto viewProjMat = simCtx.registry.try_get<CameraComponent>(renderCtx.mainCam)->viewProjMat;

        Mesh* mesh = AssetRegistry<Mesh>::GetInstance().Get(curMesh_);
        mesh->Bind();

        Shader* shader = AssetRegistry<Shader>::GetInstance().Get(curShader_);
        shader->Use();

        AssetRegistry<Texture>::GetInstance().Get(curTexture_)->Bind();

        auto group = simCtx.registry.group<TransformComponent, RenderComponent>();
        for (auto [e, trf, render] : group.each()) {
            // TODO: frustum culling

            if (curShader_ != render.shader)
            {
                curShader_ = render.shader;
                shader = AssetRegistry<Shader>::GetInstance().Get(curShader_);
                shader->Use();
            }

            if (curTexture_ != render.texture)
            {
                curTexture_ = render.texture;
                AssetRegistry<Texture>::GetInstance().Get(curTexture_)->Bind();
            }

            if (curMesh_ != render.mesh)
            {
                curMesh_ = render.mesh;
                mesh = AssetRegistry<Mesh>::GetInstance().Get(curMesh_);
                mesh->Bind();
            }

            const auto& mtx = trf.GetTransformMatrix();
            shader->SetUniformMat4("uModel", mtx);
            shader->SetUniformMat4("uViewProj", viewProjMat);
            shader->SetUniformMat3("uNormal", glm::transpose(glm::inverse(glm::mat3(mtx))));

            shader->SetUniformInt("uTexture", 0);
            shader->SetUniformVec3("uLightPos", glm::vec3(0, 10, 0));
            shader->SetUniformVec4("uColor", render.color);

            mesh->Draw();
        }
    }
}
