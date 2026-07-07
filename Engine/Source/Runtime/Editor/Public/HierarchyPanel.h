#ifndef MANGO_HIERARCHYPANEL_H
#define MANGO_HIERARCHYPANEL_H

#include <string>
#include "EditorPanel.h"
namespace tomato
{
	class HierarchyPanel : public EditorPanel
	{
	public:
		HierarchyPanel(bool open);

		void Draw(EditorContext&) override;

		const char* GetName() const { return "HierarchyPanel"; }

	private:
		void LoadResources();

		void Traverse(EditorContext& editorCtx, entt::entity e);
		void CreateAndSetHierarchyEntity(EditorContext& editorCtx, entt::entity e, bool hierarchy);

		void MenuBar(EditorContext& editorCtx);
		void AddItems(EditorContext& editorCtx, bool isPopup);
		void VisibleButton(EditorContext&, entt::entity, std::string&);
		void ToggleVisible(entt::registry& reg, entt::entity e);
		void UpdateInheritedVisibility(entt::registry& reg, entt::entity e);
		void ShowMoreButton(EditorContext&);

	private:
		unsigned int icon_visibility[2];
	};
}

#endif // !MANGO_HIERARCHYPANEL_H
