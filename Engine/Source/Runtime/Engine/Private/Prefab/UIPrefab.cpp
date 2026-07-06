#include <entt/entt.hpp>
#include "Prefab/UIPrefab.h"
#include "ECS/Components/Nametag.h"
#include "ECS/Components/UI.h"
#include "ECS/Components/Text.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/Render.h"
#include "ECS/Components/UIEvents.h"
#include "Resource/AssetHash.h"
#include "Resource/Render/Mesh.h"
#include "Resource/Render/Shader.h"
#include "Resource/Render/Texture.h"
#include "Resource/AssetRegistry.h"

#include "ECS/Entity/Hierarchy.h"
#include "Prefab/EntityUtils.h"

namespace tomato::UIPrefab
{
	entt::entity CreateCanvas(entt::registry& reg)
	{
		const entt::entity canvas = reg.create();

        reg.emplace<NametagComponent>(canvas, GenerateUUID(), GenerateEntityName(reg, "Canvas"));
        reg.emplace<tomato::CanvasComponent>(canvas);
        reg.emplace<tomato::UIComponent>(canvas, (UUID)0, 0, UIType::Canvas);
        reg.emplace<tomato::RectTransformComponent>(canvas);
        reg.emplace<tomato::RootEntityTag>(canvas);
        reg.emplace<tomato::HierarchyComponent>(canvas);
        reg.emplace<tomato::RenderComponent>(canvas,
            glm::vec4{ 1.f, 1.f, 1.f, 0.0f },
            GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::LBPlain)),
            GetAssetID("UIShader"),
            GetAssetID(Texture::PrimitiveName));

        return canvas;
	}

    entt::entity CreateButton(entt::registry& reg, glm::vec2 pos)
    {
        entt::entity canvas = GetCanvas(reg);

        const auto button = reg.create();

        reg.emplace<NametagComponent>(button, GenerateUUID(), GenerateEntityName(reg, "Button"));
        reg.emplace<UIComponent>(button, GetUUID(reg, canvas), 0);
        reg.emplace<RectTransformComponent>(button, pos, glm::vec2(0.f, 0.f), glm::vec2(0.f, 0.f), glm::vec2(100.f, 100.f), glm::vec2(0.5f, 0.5f), glm::vec2(0.5f, 0.5f), glm::vec2(0.5f, 0.5f));
        auto& selectable = reg.emplace<SelectableComponent>(button);
        reg.emplace<MouseEventComponent>(button);
        reg.emplace<HierarchyComponent>(button);
        SetHierarchy(reg, canvas, button);
        reg.emplace<RenderComponent>(button,
            selectable.normalColor,
            GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::LBPlain)),
            GetAssetID("UIShader"),
            GetAssetID(Texture::PrimitiveName));

        const auto buttonText = reg.create();
        reg.emplace<NametagComponent>(buttonText, GenerateUUID(), GenerateEntityName(reg, "Text"));
        reg.emplace<UIComponent>(buttonText, GetUUID(reg, canvas), 0, UIType::Text);
        reg.emplace<TextComponent>(buttonText, "Button", glm::vec4{ 0.3, 0.7f, 0.9f, 1.0f }, 30.f);
        reg.emplace<RectTransformComponent>(buttonText, glm::vec2(0.f, 0.f), glm::vec2(0.f, 0.f), glm::vec2(0.f, 0.f), glm::vec2(0.f, 0.f), glm::vec2(0.5f, 0.5f), glm::vec2(0.5f, 0.5f), glm::vec2(0.5f, 0.5f));
        reg.emplace<HierarchyComponent>(buttonText);
        SetHierarchy(reg, button, buttonText);

        return button;
    }

    entt::entity CreateText(entt::registry& reg, glm::vec2 pos, std::string inText, glm::vec4 color, float size, const char* fontName)
    {
        entt::entity canvas = GetCanvas(reg);

        const auto text = reg.create();

        reg.emplace<NametagComponent>(text, GenerateUUID(), GenerateEntityName(reg, "Text"));
        reg.emplace<UIComponent>(text, GetUUID(reg, canvas), 0, UIType::Text);
        reg.emplace<TextComponent>(text, inText, color, size, GetAssetID(fontName));
        reg.emplace<RectTransformComponent>(text, pos, glm::vec2(0.f, 0.f), glm::vec2(0.f, 0.f), glm::vec2(0.f, 0.f), glm::vec2(0.5f, 0.5f), glm::vec2(0.5f, 0.5f), glm::vec2(0.5f, 0.5f));
        reg.emplace<HierarchyComponent>(text);
        SetHierarchy(reg, canvas, text);

        return text;
    }

    entt::entity CreateImage(entt::registry& reg, const char* textureName, glm::vec2 pos, glm::vec2 size)
    {
        entt::entity canvas = GetCanvas(reg);
        
        const auto img = reg.create();

        reg.emplace<NametagComponent>(img, GenerateUUID(), GenerateEntityName(reg, "Image"));
        reg.emplace<UIComponent>(img, GetUUID(reg, canvas), 0);

        auto texture = AssetRegistry<Texture>::GetInstance().Get(GetAssetID(textureName));
        if (textureName == Texture::PrimitiveName)
            size = glm::vec2{ 100.f, 100.f };

        reg.emplace<RectTransformComponent>(
            img,
            pos,
            glm::vec2(0.f, 0.f),
            glm::vec2(0.f, 0.f),
            size == glm::vec2(0.f, 0.f) ? glm::vec2{texture->GetWidth(), texture->GetHeight()} : glm::vec2{100.f, 100.f},
            glm::vec2(0.5f, 0.5f),
            glm::vec2(0.5f, 0.5f),
            glm::vec2(0.5f, 0.5f)
        );

        reg.emplace<RenderComponent>(
            img,
            glm::vec4{ 1.f, 1.f, 1.f, 1.f },
            GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::LBPlain)),
            GetAssetID("UIShader"),
            GetAssetID(textureName));
        reg.emplace<HierarchyComponent>(img);
        SetHierarchy(reg, canvas, img);
        
        return img;
    }

    entt::entity GetCanvas(entt::registry& reg)
    {
        std::vector<entt::entity> canvases;
        auto canvasView = reg.view<CanvasComponent>();

        if (canvasView.empty())
            return CreateCanvas(reg);
        
        else
        {
            for (auto canvas : canvasView)
                canvases.push_back(canvas);

            std::sort(canvases.begin(), canvases.end(),
                [&](entt::entity a, entt::entity b)
                {
                    return reg.get<CanvasComponent>(a).sortOrder <
                        reg.get<CanvasComponent>(b).sortOrder;
                });

            return canvases[0];
        }
    }
}