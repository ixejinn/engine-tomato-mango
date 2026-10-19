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
#include "ECS/Components/UIDirty.h"

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
        reg.emplace<UIComponent>(uiEntity, canvas, 0, type);
        auto& rect = reg.emplace<tomato::RectTransformComponent>(uiEntity);
        rect.SetPosition({ 0.f, 0.f });
        rect.SetOffsetMin({ 0.f, 0.f }); rect.SetOffsetMax({ 0.f, 0.f });
        rect.SetSize({ 0.f, 0.f });
        rect.SetAnchor({ 0.5f, 0.5f }, { 0.5f, 0.5f });
        rect.SetPivot({ 0.5f, 0.5f });
        rect.dirty = UI::Dirty::All;

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
        auto& render = reg.emplace<RenderComponent>(button);
        render.color = selectable.normalColor;
        render.mesh = GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::LBPlain));
        render.shader = GetAssetID("UIShader");

        auto& rect = reg.get<RectTransformComponent>(button);
        rect.SetPosition({ pos });
        rect.SetSize({ 100.f, 100.f });

        const entt::entity buttonText = CreateText(reg, canvas, { 0.f, 0.f }, "Button", { 0.9f, 0.32f, 0.3f, 1.0f }, 30.f);

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

        auto& rect = reg.get<RectTransformComponent>(text);
        rect.SetPosition({ pos });
        rect.SetSize({ 0.f, 0.f });

        return text;
    }

    entt::entity CreateImage(entt::registry& reg, entt::entity canvas, const std::filesystem::path& textureName, glm::vec2 pos, glm::vec2 size)
    {
        canvas = canvas == entt::null ? GetCanvas(reg) : canvas;

        const entt::entity img = CreateBaseUIEntity(reg, "Image", canvas);

        auto texture = AssetRegistry<Texture>::GetInstance().Get(GetAssetID(textureName));
        if (textureName == Texture::PrimitiveName)
            size = glm::vec2{ 100.f, 100.f };

        auto& rect = reg.get<RectTransformComponent>(img);
        rect.SetPosition({ pos });
        rect.SetSize(size == glm::vec2(0.f, 0.f) ?
            glm::vec2{ texture->GetWidth(), texture->GetHeight() } : glm::vec2{ 100.f, 100.f });

        auto& render = reg.emplace<RenderComponent>(img);
        render.mesh = GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::LBPlain));
        render.shader = GetAssetID("UIShader");
        render.texture = GetAssetID(textureName);

        return img;
    }

    entt::entity GetCanvas(entt::registry& reg, RenderMode mode)
    {
        std::vector<entt::entity> canvases;
        auto canvasView = reg.view<CanvasComponent>();

        if (canvasView.empty())
            return CreateCanvas(reg);
        
        else
        {
            for (auto [e, canvas] : canvasView.each())
            {
                if (mode == canvas.mode)
                    return e;
            }
        }
        return CreateCanvas(reg);
    }
}