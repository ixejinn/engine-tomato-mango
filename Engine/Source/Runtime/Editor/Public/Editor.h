#ifndef MANGO_EDITOR_H
#define MANGO_EDITOR_H

#include <vector>
#include <memory>
#include <glm/fwd.hpp>
#include <entt/fwd.hpp>
#include <State/StateFwd.h>

#include "EditorPanel.h"
#include "MainMenuBar.h"

struct GLFWwindow;
namespace tomato
{
	class Editor
	{
	public:
		void InitImGui(GLFWwindow* wnd);
		void ShutdownImGui();

		void BeginFrame();
		void Draw(State*, RunMode&);
		void EndFrame();

		void SetInputCallbacks();

		void PickObject(entt::registry& reg, glm::vec2 mousePos);

	private:
		void LoadResources();
		void ResetEditorContext(State*);
	private:
		std::vector<std::unique_ptr<EditorPanel>> panels;
		MainMenuBar mainMenu_;

		EditorContext eCtx;
		//entt::entity selectedEntity;
	};
}

#endif // !MANGO_EDITOR_H
