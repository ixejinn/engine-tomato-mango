#include "ECS/Systems/WorldUIRenderSystem.h"
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
	WorldUIRenderSystem::WorldUIRenderSystem()
	{
		textRenderer_.Init();
	}

	void WorldUIRenderSystem::Update(SimContext& simCtx)
	{
        auto& registry = simCtx.state->GetRegistry();
        auto* uiCtx = registry.ctx().find<UIContext>();
        if (!uiCtx)
            return;

        auto& renderCtx = registry.ctx().get<RenderContext>();
        auto& gl = renderCtx.glState;

        PipelineState state{ PipelinePresets::ScreenUI };
        state.depth.testEnabled = true;
        gl.Apply(state);

        auto* camera = registry.try_get<CameraComponent>(renderCtx.mainCam);

        if (!camera) return;

        auto& shaderRegistry = AssetRegistry<Shader>::GetInstance();
        auto& texRegistry = AssetRegistry<Texture>::GetInstance();
        auto& meshRegistry = AssetRegistry<Mesh>::GetInstance();

        for (auto e : uiCtx->worldDrawList)
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
                shader->SetUniformMat4("uView", camera->view);
                shader->SetUniformMat4("uProjection", camera->projection);
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
            FontShader->SetUniformInt("text", 0);
        FontShader->SetUniformMat4("projection", camera->viewProjMat);

        for (auto e : uiCtx->worldDrawList)
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