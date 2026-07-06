#ifndef MANGO_INSPECTORPANEL_H
#define MANGO_INSPECTORPANEL_H

#include "EditorPanel.h"

namespace tomato
{
	namespace Serialization
	{
		enum class ComponentCategory;
	}

	class InspectorPanel : public EditorPanel
	{
	public:
		InspectorPanel(bool open) : EditorPanel(open) {}

		void Draw(EditorContext&) override;

		const char* GetName() const { return "InspectorPanel"; }

	private:
		void MenuBar(EditorContext&);
		void ShowAddComponent(EditorContext&, Serialization::ComponentCategory);
	};
}

#endif // !MANGO_INSPECTORPANEL_H
