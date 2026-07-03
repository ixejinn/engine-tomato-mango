#ifndef MANGO_UI_EVENTS_H
#define MANGO_UI_EVENTS_H

#include <entt/fwd.hpp>

namespace tomato
{
	struct MouseClickEvent
	{
		entt::entity e;
		entt::registry* reg;
        int64_t tick;
	};

	using UIMouseEventCallBack = std::function<void(const MouseClickEvent&)>;

	struct MouseEventComponent
	{
		UIMouseEventCallBack onClick{ nullptr };
	};
}

#endif // !MANGO_UI_EVENTS_H
