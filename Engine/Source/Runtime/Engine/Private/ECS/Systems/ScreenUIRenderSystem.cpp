#include "ECS/Systems/ScreenUIRenderSystem.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"

#include "Services/Window.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

#include "Render/GLStateCache.h"
namespace tomato
{
	ScreenUIRenderSystem::ScreenUIRenderSystem()
	{
        AssetRegistry<Font>::GetInstance().CreatePrimitives();
        textRenderer_.Init();
	}

	void ScreenUIRenderSystem::Update(SimContext& simCtx)
	{
	    auto& registry = simCtx.state->GetRegistry();
        auto* uiCtx = registry.ctx().find<UIContext>();
        if (uiCtx == nullptr)
            return;

        auto& gl = registry.ctx().get<RenderContext>().glState;
        gl.Apply(PipelinePresets::ScreenUI);

        auto& shaderRegistry = AssetRegistry<Shader>::GetInstance();
        auto& texRegistry = AssetRegistry<Texture>::GetInstance();
        auto& meshRegistry = AssetRegistry<Mesh>::GetInstance();

        glm::mat4 projection = glm::ortho(
            0.0f, static_cast<float>(Window::GetWidth()),
            0.0f, static_cast<float>(Window::GetHeight()),
            -1.0f, 1.0f);

        for (auto e : uiCtx->screenDrawList)
        {
            if (!IsVisible(registry, e))
                continue;

            auto& ui = registry.get<UIComponent>(e);
            if (ui.type == UIType::Text)
                continue; //text

            auto* render = registry.try_get<RenderComponent>(e);
            if (!render) continue;

            Shader* shader = shaderRegistry.Get(render->shader);
            Mesh* mesh = meshRegistry.Get(render->mesh);
            Texture* texture = texRegistry.Get(render->texture);

            if (gl.UseShader(shader->GetHandle()))
            {
                shader->SetUniformInt("tex", 0);
                shader->SetUniformMat4("uView", glm::mat4(1.0f));
                shader->SetUniformMat4("uProjection", projection);
            }

            auto& rect = registry.get<RectTransformComponent>(e);
            shader->SetUniformMat4("uModel", rect.model_matrix);
            shader->SetUniformVec4("uColor", render->color);

            gl.BindTexture(texture->GetHandle());
            gl.BindVertexArray(mesh->GetHandle());

            mesh->Draw();
        }

        //TextComponent Render
        Shader* FontShader = shaderRegistry.Get(GetAssetID("FontShader"));
        if (gl.UseShader(FontShader->GetHandle()))
        {
            FontShader->SetUniformInt("text", 0);
            FontShader->SetUniformMat4("projection", projection);
        }

        for (auto e : uiCtx->screenDrawList)
        {
            if (!IsVisible(registry, e))
                continue;

            auto& ui = registry.get<UIComponent>(e);
            if (ui.type != UIType::Text)
                continue; //text

            auto& text = registry.get<TextComponent>(e);
            Font* font = AssetRegistry<Font>::GetInstance().Get(text.font);
            if (!font)
                continue;

            auto& rect = registry.get<RectTransformComponent>(e);
            glm::vec2 pivotOffset = -(rect.computedSize * rect.pivot);

            textRenderer_.DrawString(
                gl,
                text.codepoints,
                pivotOffset.x, pivotOffset.y,
                text.fontSize / 64.f,
                text.color,
                font,
                rect.model_matrix
            );
        }
        textRenderer_.Flush(gl);
	}
}