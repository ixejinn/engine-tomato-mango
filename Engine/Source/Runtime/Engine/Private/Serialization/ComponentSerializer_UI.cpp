#include "Serialization/ComponentSerializer.h"

#include "ECS/Components/UI.h"
#include "ECS/Components/UIEvents.h"
#include "ECS/Components/UIDirty.h"
#include "ECS/Components/Text.h"

#include "ECS/Entity/Entity.h"

namespace tomato::Serialization
{
	void Save(json& data, entt::registry& registry, const UIComponent& ui)
	{
		data["canvas"] = GetUUID(registry, ui.canvas);
		data["sortOrder"] = ui.sortOrder;
		data["type"] = ui.type;
	}

	void Load(const json& data, entt::registry& registry, UIComponent& ui)
	{
		ui.canvas = GetEntityByUUID(registry, data["canvas"]);
		ui.sortOrder = data["sortOrder"];
		ui.type = data["type"];
	}

	void Save(json& data, entt::registry& registry, const CanvasComponent& canvas)
	{
		data["mode"] = canvas.mode;
		data["refSize"] = { canvas.referenceSize.x, canvas.referenceSize.y };
		data["actSize"] = { canvas.actualSize.x, canvas.actualSize.y };
		data["sortOrder"] = canvas.sortOrder;
	}

	void Load(const json& data, entt::registry& registry, CanvasComponent& canvas)
	{
		canvas.mode = data["mode"];
		canvas.referenceSize = { data["refSize"][0], data["refSize"][1] };
		canvas.actualSize = { data["actSize"][0], data["actSize"][1] };
		canvas.sortOrder = data["sortOrder"];
	}

	void Save(json& data, entt::registry& registry, const RectTransformComponent& rectTransform)
	{
		data["anchorPos"] = { rectTransform.anchoredPosition.x, rectTransform.anchoredPosition.y };
		data["offsetMin"] = { rectTransform.offsetMin.x, rectTransform.offsetMin.y };
		data["offsetMax"] = { rectTransform.offsetMax.x, rectTransform.offsetMax.y };
		data["sizeDelta"] = { rectTransform.sizeDelta.x, rectTransform.sizeDelta.y };
		data["anchorMin"] = { rectTransform.anchorMin.x, rectTransform.anchorMin.y };
		data["anchorMax"] = { rectTransform.anchorMax.x, rectTransform.anchorMax.y };
		data["pivot"] = { rectTransform.pivot.x, rectTransform.pivot.y };
	}

	void Load(const json& data, entt::registry& registry, RectTransformComponent& rectTransform)
	{
		rectTransform.anchoredPosition = { data["anchorPos"][0], data["anchorPos"][1] };
		rectTransform.offsetMin = { data["offsetMin"][0], data["offsetMin"][1] };
		rectTransform.offsetMax = { data["offsetMax"][0], data["offsetMax"][1] };
		rectTransform.sizeDelta = { data["sizeDelta"][0], data["sizeDelta"][1] };
		rectTransform.anchorMin = { data["anchorMin"][0], data["anchorMin"][1] };
		rectTransform.anchorMax = { data["anchorMax"][0], data["anchorMax"][1] };
		rectTransform.pivot = { data["pivot"][0], data["pivot"][1] };

		rectTransform.dirty = UI::Dirty::All;
	}

	void Save(json& data, entt::registry& registry, const TextComponent& text)
	{
		data["text"] = text.text;
		data["color"] = { text.color.x, text.color.y, text.color.z, text.color.w };
		data["fontSize"] = text.fontSize;
		data["font"] = text.font;
	}

	void Load(const json& data, entt::registry& registry, TextComponent& text)
	{
		text.text = data["text"];
		text.color = {
			data["color"][0],
			data["color"][1],
			data["color"][2],
			data["color"][3],
		};
		text.fontSize = data["fontSize"];
		text.font = data["font"];
	}

	void Save(json& data, entt::registry& registry, const MouseEventComponent& mouseEvt) {}
	void Load(const json& data, entt::registry& registry, MouseEventComponent& mouseEvt) {}

	void Save(json& data, entt::registry& registry, const SelectableComponent& selectable)
	{
		data["interactable"] = selectable.interactable;

		data["normal"] = { selectable.normalColor.x, selectable.normalColor.y, selectable.normalColor.z, selectable.normalColor.w };
		data["highlight"] = { selectable.highlightedColor.x, selectable.highlightedColor.y, selectable.highlightedColor.z, selectable.highlightedColor.w };
		data["pressed"] = { selectable.pressedColor.x, selectable.pressedColor.y, selectable.pressedColor.z, selectable.pressedColor.w };
	}

	void Load(const json& data, entt::registry& registry, SelectableComponent& selectable)
	{
		selectable.interactable = data["interactable"];

		selectable.normalColor = {
			data["normal"][0],
			data["normal"][1],
			data["normal"][2],
			data["normal"][3]
		};

		selectable.highlightedColor = {
			data["highlight"][0],
			data["highlight"][1],
			data["highlight"][2],
			data["highlight"][3]
		};

		selectable.pressedColor = {
			data["pressed"][0],
			data["pressed"][1],
			data["pressed"][2],
			data["pressed"][3]
		};
	}
}