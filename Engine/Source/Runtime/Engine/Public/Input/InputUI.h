#ifndef MANGO_INPUTUI_H
#define MANGO_INPUTUI_H

#include <glm/vec2.hpp>

#include "Input/KeyConstants.h"
#include "Input/InputEvent.h"
#include "Event/EventSignal.h"
#include "State/StateFwd.h"

namespace tomato
{
	class InputUI
	{
	public:
		bool OnClick(const MouseEvent& mouseEvent);
		bool OnHover(const MouseMoveEvent& moveEvent);

		void SetState(State* newState);

	private:
		bool PointInRect(glm::vec2 point, glm::vec2 min, glm::vec2 max);
		entt::entity PickSelectable(glm::vec2 point);

		entt::entity currentHovered{ entt::null };
		entt::entity previousHovered{ entt::null };
		entt::entity pressed{ entt::null };

	private:
		State* currentStatePtr_{ nullptr };
	};
}


#endif // !MANGO_INPUTUI_H
