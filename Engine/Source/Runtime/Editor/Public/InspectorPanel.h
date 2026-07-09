#ifndef MANGO_INSPECTORPANEL_H
#define MANGO_INSPECTORPANEL_H

#include <string>
#include "EditorPanel.h"

namespace tomato
{
	namespace Serialization
	{
		enum class ComponentCategory;
		struct ComponentInfo;
	}

	class InspectorPanel : public EditorPanel
	{
	public:
		InspectorPanel(bool open);

		void Draw(EditorContext&) override;

		const char* GetName() const { return "InspectorPanel"; }

	private:
		void LoadResources();

		void ShowEntityUID(EditorContext&);

		void MenuBar(EditorContext&);
		void ShowAddComponent(EditorContext&, Serialization::ComponentCategory);
		void MoreButton(EditorContext&, const Serialization::ComponentInfo&);

	private:
		unsigned int more_vert;
	};
}

#endif // !MANGO_INSPECTORPANEL_H
