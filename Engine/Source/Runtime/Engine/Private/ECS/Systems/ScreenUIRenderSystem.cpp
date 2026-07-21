#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ECS/Systems/ScreenUIRenderSystem.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"

#include "ECS/Components/Camera.h"
#include "ECS/Components/Render.h"
#include "ECS/Components/Visibility.h"
#include "ECS/Components/Text.h"
#include "ECS/Components/UI.h"
#include "ECS/Entity/Entity.h"

#include "Resource/AssetHash.h"
#include "Resource/AssetRegistry.h"
#include "Resource/Render/Mesh.h"
#include "Resource/Render/Shader.h"
#include "Resource/Render/Texture.h"
#include "Resource/Render/Font.h"

#include "Services/Window.h"

namespace tomato
{
	ScreenUIRenderSystem::ScreenUIRenderSystem()
    : curMesh_(GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::LBPlain)))
    , curShader_(GetAssetID("UIShader"))
    , curTexture_(GetAssetID(Texture::PrimitiveName))
	{
        AssetRegistry<Font>::GetInstance().CreatePrimitives();
        textRenderer_.Init(AssetRegistry<Shader>::GetInstance().Get(GetAssetID("FontShader")));
	}

	void ScreenUIRenderSystem::Update(SimContext& simCtx)
	{
        glDisable(GL_DEPTH_TEST);

	    auto& registry = simCtx.state->GetRegistry();
        auto* uiCtx = registry.ctx().find<UIContext>();
        if (uiCtx == nullptr)
            return;

        Mesh* mesh = AssetRegistry<Mesh>::GetInstance().Get(curMesh_);
        mesh->Bind();

        Shader* shader = AssetRegistry<Shader>::GetInstance().Get(curShader_);
        shader->Use();

        glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(Window::GetWidth()), 0.0f, static_cast<float>(Window::GetHeight()), -1.0f, 1.0f);
        shader->SetUniformInt("tex", 0);
        shader->SetUniformMat4("projection", projection);

        AssetRegistry<Texture>::GetInstance().Get(curTexture_)->Bind();

        for (auto e : uiCtx->drawList)
        {
            auto& ui = registry.get<UIComponent>(e);
            if (ui.type == UIType::Text)
                continue; //text

            auto& rect = registry.get<RectTransformComponent>(e);
            auto* render = registry.try_get<RenderComponent>(e);
            if (!render) continue;

            if (!IsVisible(registry, e))
                continue;

            if (curShader_ != render->shader)
            {
                curShader_ = render->shader;
                shader = AssetRegistry<Shader>::GetInstance().Get(curShader_);
                shader->Use();

                shader->SetUniformInt("tex", 0);
                shader->SetUniformMat4("projection", projection);
            }

            if (curTexture_ != render->texture)
            {
                curTexture_ = render->texture;
                AssetRegistry<Texture>::GetInstance().Get(curTexture_)->Bind();
            }

            if (curMesh_ != render->mesh)
            {
                curMesh_ = render->mesh;
                mesh = AssetRegistry<Mesh>::GetInstance().Get(curMesh_);
                mesh->Bind();
            }

            shader->SetUniformMat4("uModel", rect.model_matrix);
            shader->SetUniformVec4("uColor", render->color);

            mesh->Draw();
        }

        //TextComponent Render
        shader = AssetRegistry<Shader>::GetInstance().Get(GetAssetID("FontShader"));
        shader->Use();
        shader->SetUniformInt("text", 0);

        for (auto e : uiCtx->drawList)
        {
            auto& ui = registry.get<UIComponent>(e);
            if (ui.type != UIType::Text)
                continue; //text

            auto& text = registry.get<TextComponent>(e);
            auto& rect = registry.get<RectTransformComponent>(e);

            if (!IsVisible(registry, e))
                continue;

            Font* font = AssetRegistry<Font>::GetInstance().Get(text.font);
            if (!font)
                continue;
            glm::vec2 pivotOffset = -(rect.computedSize * rect.pivot);

            textRenderer_.DrawString(
                text.codepoints,
                pivotOffset.x, pivotOffset.y,
                text.fontSize / 64.f,
                text.color,
                font,
                rect.model_matrix
            );
        }
        textRenderer_.Flush();

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
	}
}