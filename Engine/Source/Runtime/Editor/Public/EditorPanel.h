#ifndef MANGO_EDITORPANEL_H
#define MANGO_EDITORPANEL_H

#include <entt/fwd.hpp>
#include <State/StateFwd.h>

namespace tomato
{
	struct EditorContext
	{
		State* currentState;
		entt::entity selectedEntity;
	};

	class EditorPanel
	{
	public:
		EditorPanel(bool open) : open(open) {}

		virtual const char* GetName() const = 0;
		virtual void Draw(EditorContext&) = 0;

		bool open{ false };
	};
	
}
#endif // !MANGO_EDITORPANEL_H
