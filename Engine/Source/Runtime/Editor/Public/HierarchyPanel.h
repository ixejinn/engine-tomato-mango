#ifndef MANGO_HIERARCHYPANEL_H
#define MANGO_HIERARCHYPANEL_H

#include "EditorPanel.h"

namespace tomato
{
	namespace Serialization
	{
		enum class ComponentCategory;
	}

	class HierarchyPanel : public EditorPanel
	{
	public:
		HierarchyPanel(bool open) : EditorPanel(open) {}

		void Draw(EditorContext&) override;

		const char* GetName() const { return "HierarchyPanel"; }

	private:
		void Traverse(EditorContext& editorCtx, entt::entity e);

		void MenuBar(EditorContext& editorCtx);
		void ShowAddComponent(EditorContext&, Serialization::ComponentCategory);
	};
}

#endif // !MANGO_HIERARCHYPANEL_H
