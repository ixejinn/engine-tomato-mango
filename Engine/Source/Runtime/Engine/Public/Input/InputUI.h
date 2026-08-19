#ifndef MANGO_INPUTUI_H
#define MANGO_INPUTUI_H

#include <entt/entt.hpp>
#include <glm/vec2.hpp>
#include "Input/KeyConstants.h"
#include "Input/InputEventFwd.h"
#include "State/StateFwd.h"
#include "ECS/SystemFramework/SystemConstants.h"
#include "ECS/SystemFramework/SystemFrameworkEventFwd.h"

namespace tomato
{
	class InputUI
	{
	public:
        InputUI();

		bool OnClick(const MouseButtonEvent& mouseEvent);
		bool OnHover(const CursorPosEvent& moveEvent);

		void SetState(State* newState);

	private:
        void OnChangeRunMode(const ChangeRunModeEvent& modeEvent);

		bool PointInRect(glm::vec2 point, glm::vec2 min, glm::vec2 max);
		entt::entity PickSelectable(glm::vec2 point);

		entt::entity currentHovered{ entt::null };
		entt::entity previousHovered{ entt::null };
		entt::entity pressed{ entt::null };

	private:
		State* currentStatePtr_{ nullptr };

        RunMode runMode_{RunMode::None};
	};
}


#endif // !MANGO_INPUTUI_H
