#ifndef MANGO_MAINMENUBAR_H
#define MANGO_MAINMENUBAR_H

#include "ECS/SystemFramework/SystemConstants.h"

namespace tomato
{
	struct EditorContext;

	enum class PendingAction
	{
		None,
		NewScene,
		OpenScene,
		Exit,
	};

	class MainMenuBar
	{
	public:
		void Draw(EditorContext&, RunMode&);

	private:
		void MenuFile(EditorContext&);

		void NewScene(EditorContext&);
		void OpenScene(EditorContext&);
		void Save(EditorContext&);
		void SaveAs(EditorContext&);
		void ReLoadScene(EditorContext&);

		void OpenPopupModal(EditorContext&);
		void ExecutePendingAction(EditorContext&, bool);

	private:
		PendingAction pendingAction_ = PendingAction::None;
		bool openNotSavedPopup = false;

		bool modeBtnClicked = false;
	};
}
#endif // !MANGO_MAINMENUBAR_H
