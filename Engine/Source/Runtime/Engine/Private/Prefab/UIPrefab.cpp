#include <entt/entt.hpp>

#include "Prefab/UIPrefab.h"

#include "ECS/Components/Nametag.h"
#include "ECS/Components/Activetag.h"
#include "ECS/Components/Visibility.h"
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
#include "ECS/Entity/Entity.h"

namespace tomato::UIPrefab
{
    entt::entity CreateBaseUIEntity(entt::registry& reg, const std::string& name, entt::entity canvas, UIType type)
    {
        const entt::entity uiEntity = reg.create();

        auto& generator = reg.ctx().get<EntityNameGenerator>();
        reg.emplace<NametagComponent>(uiEntity, GenerateUUID(), generator.Generate(name));
        reg.emplace<tomato::UIComponent>(uiEntity, GetUUID(reg, canvas), 0, type);
        reg.emplace<tomato::RectTransformComponent>(uiEntity);

        reg.emplace<tomato::VisibilityComponent>(uiEntity);
        reg.emplace<tomato::ActiveTag>(uiEntity);

        if(canvas != entt::null)
        {
            reg.emplace<HierarchyComponent>(uiEntity);
            SetHierarchy(reg, canvas, uiEntity);
        }

        return uiEntity;
    }

	entt::entity CreateCanvas(entt::registry& reg, RenderMode mode)
	{
        const entt::entity canvas = CreateBaseUIEntity(reg, "Canvas", entt::null, UIType::Canvas);
        reg.emplace<tomato::CanvasComponent>(canvas, mode);
        reg.emplace<tomato::RootEntityTag>(canvas);
        reg.emplace<tomato::HierarchyComponent>(canvas);

        return canvas;
	}

    entt::entity CreateButton(entt::registry& reg, entt::entity canvas, glm::vec2 pos)
    {
        canvas = canvas == entt::null ? GetCanvas(reg) : canvas;

        const entt::entity button = CreateBaseUIEntity(reg, "Button", canvas);

        auto& selectable = reg.emplace<SelectableComponent>(button);
        reg.emplace<MouseEventComponent>(button);
        reg.emplace<RenderComponent>(button,
                selectable.normalColor,
                GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::LBPlain)),
                GetAssetID("UIShader"),
                GetAssetID(Texture::PrimitiveName));
        reg.emplace_or_replace<RectTransformComponent>(button,
            pos, glm::vec2(0.f, 0.f), glm::vec2(0.f, 0.f),
            glm::vec2(100.f, 100.f), glm::vec2(0.5f, 0.5f),
            glm::vec2(0.5f, 0.5f), glm::vec2(0.5f, 0.5f));

        const entt::entity buttonText = CreateText(reg, canvas, { 0.f, 0.f }, "Button", { 0.3, 0.7f, 0.9f, 1.0f }, 30.f);

        SetHierarchy(reg, button, buttonText);

        return button;
    }

    entt::entity CreateText(entt::registry& reg, entt::entity canvas, glm::vec2 pos, std::string inText, glm::vec4 color, float size, const std::filesystem::path& fontName)
    {
        canvas = canvas == entt::null ? GetCanvas(reg) : canvas;

        const entt::entity text = CreateBaseUIEntity(reg, "Text", canvas, UIType::Text);

        auto& txtComp = reg.emplace<TextComponent>(text, inText);
        txtComp.color = color;
        txtComp.fontSize = size;
        txtComp.font = GetAssetID(fontName);

        reg.emplace_or_replace<RectTransformComponent>(text,
            pos, glm::vec2(0.f, 0.f), glm::vec2(0.f, 0.f),
            glm::vec2(0.f, 0.f), glm::vec2(0.5f, 0.5f),
            glm::vec2(0.5f, 0.5f), glm::vec2(0.5f, 0.5f));

        return text;
    }

    entt::entity CreateImage(entt::registry& reg, entt::entity canvas, const std::filesystem::path& textureName, glm::vec2 pos, glm::vec2 size)
    {
        canvas = canvas == entt::null ? GetCanvas(reg) : canvas;

        const entt::entity img = CreateBaseUIEntity(reg, "Image", canvas);

        auto texture = AssetRegistry<Texture>::GetInstance().Get(GetAssetID(textureName));
        if (textureName == Texture::PrimitiveName)
            size = glm::vec2{ 100.f, 100.f };

        reg.emplace_or_replace<RectTransformComponent>(
            img,
            pos,
            glm::vec2(0.f, 0.f),
            glm::vec2(0.f, 0.f),
            size == glm::vec2(0.f, 0.f) ? glm::vec2{ texture->GetWidth(), texture->GetHeight() } : glm::vec2{ 100.f, 100.f },
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