#ifndef MANGO_HIERARCHYPANEL_H
#define MANGO_HIERARCHYPANEL_H

#include "EditorPanel.h"

namespace tomato
{
	class HierarchyPanel : public EditorPanel
	{
	public:
		HierarchyPanel(bool open) : EditorPanel(open) {}

		void Draw(EditorContext&) override;

		const char* GetName() const { return "HierarchyPanel"; }

	private:
		void Traverse(EditorContext& editorCtx, entt::entity e);
	};
}

#endif // !MANGO_HIERARCHYPANEL_H
