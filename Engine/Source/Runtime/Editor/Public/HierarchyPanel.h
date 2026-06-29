#ifndef MANGO_ENTITYPANEL_H
#define MANGO_ENTITYPANEL_H

#include "EditorPanel.h"

namespace tomato
{
	class HierarchyPanel : public EditorPanel
	{
	public:
		void Draw(EditorContext&) override;

		const char* GetName() const { return "EntityPanel"; }

		bool isOpened() const { return open; }

	private:

		void Traverse(EditorContext& editorCtx, entt::entity e);
		bool open{ true };
	};
}

#endif // !MANGO_ENTITYPANEL_H
