#ifndef MANGO_INSPECTORPANEL_H
#define MANGO_INSPECTORPANEL_H

#include "EditorPanel.h"

namespace tomato
{
	class InspectorPanel : public EditorPanel
	{
	public:
		void Draw(EditorContext&) override;

		const char* GetName() const { return "InspectorPanel"; }
		
		bool isOpened() const { return open; }

	private:
		bool open{ true };
	};
}

#endif // !MANGO_INSPECTORPANEL_H
