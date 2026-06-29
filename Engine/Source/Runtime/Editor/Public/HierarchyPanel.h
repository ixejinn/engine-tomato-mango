#ifndef MANGO_HIERARCHYPANEL_H
#define MANGO_HIERARCHYPANEL_H

#include "EditorPanel.h"

namespace tomato
{
	class HierarchyPanel : public EditorPanel
	{
	public:
		void Draw(EditorContext&) override;

		const char* GetName() const { return "HierarchyPanel"; }

		bool isOpened() const { return open; }

	private:

		void Traverse(EditorContext& editorCtx, entt::entity e);
		bool open{ true };
	};
}

#endif // !MANGO_HIERARCHYPANEL_H
