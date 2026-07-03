#include <GLFW/glfw3.h>

#include "Input/InputUI.h"
#include "ECS/SystemUpdateContexts.h"
#include "Services/Input.h"
#include "Services/Window.h"
#include "ECS/Components/Render.h"
#include "ECS/Components/UI.h"
#include "ECS/Components/UIEvents.h"
#include "State/State.h"

namespace tomato
{
	bool InputUI::OnClick(const MouseEvent& mouseEvent)
	{
		if (currentStatePtr_ == nullptr)
			return true;

		auto& r = currentStatePtr_->GetRegistry();

		if (currentHovered == entt::null)
			return true;

		auto& rect = r.get<RectTransformComponent>(currentHovered);
		auto& selectable = r.get<SelectableComponent>(currentHovered);
		auto& render = r.get<RenderComponent>(currentHovered);
		if (!r.all_of<MouseEventComponent>(currentHovered))
			return false;
		auto& mouseEvnt = r.get<MouseEventComponent>(currentHovered);

		if (mouseEvent.action == KeyAction::Press)
		{
			pressed = currentHovered;
			render.color = selectable.pressedColor;
		}

		if (mouseEvent.action == KeyAction::Release)
		{
			render.color = selectable.normalColor;

			if (pressed == currentHovered && mouseEvnt.onClick)
				mouseEvnt.onClick(MouseClickEvent{ currentHovered, &r , mouseEvent.tick});
		}

		return false;
	}

	bool InputUI::OnHover(const MouseMoveEvent& moveEvent)
	{
		if (currentStatePtr_ == nullptr)
			return true;

		auto& r = currentStatePtr_->GetRegistry();

		previousHovered = currentHovered;
		currentHovered = PickSelectable(glm::vec2{ moveEvent.xPos, moveEvent.yPos });
		if (currentHovered == entt::null)
			return true;

		if (previousHovered == currentHovered && Input::IsKeyPressed(Key::LeftMouseButton))
			return false;

		auto& rect = r.get<RectTransformComponent>(currentHovered);
		auto& selectable = r.get<SelectableComponent>(currentHovered);
		auto& render = r.get<RenderComponent>(currentHovered);

		render.color = selectable.highlightedColor;

		return false;
	}

	void InputUI::SetState(State* newState)
	{
		currentStatePtr_ = newState;
	}

	bool InputUI::PointInRect(glm::vec2 point, glm::vec2 min, glm::vec2 max)
	{
		return
			point.x >= min.x &&
			point.x <= max.x &&
			point.y >= min.y &&
			point.y <= max.y;
	}

	entt::entity InputUI::PickSelectable(glm::vec2 point)
	{
		if (currentStatePtr_ == nullptr)
			return entt::null;

		auto& r = currentStatePtr_->GetRegistry();
		float windowHeight = Window::GetHeight();
		
		auto* uiCtx = r.ctx().find<UIContext>();
		if (uiCtx == nullptr)
			return entt::null;

		for (auto it = uiCtx->selectableList.begin(); it != uiCtx->selectableList.end(); ++it)
		{
			if (!r.all_of<SelectableComponent>(*it)) continue;

			auto& rect = r.get<RectTransformComponent>(*it);
			auto& button = r.get<SelectableComponent>(*it);
			auto& render = r.get<RenderComponent>(*it);

			if (button.interactable)
			{
				if (PointInRect(glm::vec2(point.x, windowHeight - point.y), rect.screenRect.min, rect.screenRect.max))
					return *it;
				else
					render.color = button.normalColor;
			}
		}

		return entt::null;
	}
}
