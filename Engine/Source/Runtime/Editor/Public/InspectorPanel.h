#ifndef MANGO_INSPECTORPANEL_H
#define MANGO_INSPECTORPANEL_H

#include "EditorPanel.h"

namespace tomato
{
	class InspectorPanel : public EditorPanel
	{
	public:
		InspectorPanel(bool open) : EditorPanel(open) {}

		void Draw(EditorContext&) override;

		const char* GetName() const { return "InspectorPanel"; }
	};
}

#endif // !MANGO_INSPECTORPANEL_H
