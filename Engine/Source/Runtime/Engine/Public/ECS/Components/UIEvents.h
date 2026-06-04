#ifndef MANGO_UI_EVENTS_H
#define MANGO_UI_EVENTS_H

#include <entt/fwd.hpp>

namespace tomato
{
	struct MouseEnterEvent
	{
		entt::entity e;
		entt::registry* reg;
	};

	using UIEventCallBack = std::function<void(const MouseEnterEvent&)>;

	struct MouseEventComponent
	{
		UIEventCallBack onClick{ nullptr };
	};
}

#endif // !MANGO_UI_EVENTS_H
