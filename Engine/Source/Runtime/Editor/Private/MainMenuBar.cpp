#include "MainMenuBar.h"

#include <string>
#include <entt/entt.hpp>

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "EditorPanel.h"
#include "State/State.h"
#include "Serialization/ComponentSerializer.h"

#include "Utils/FileDialog.h"

namespace tomato
{
	void MainMenuBar::Draw(EditorContext& eCtx)
	{
		if (ImGui::BeginMainMenuBar())
		{
			MenuFile(eCtx);
			ImGui::EndMainMenuBar();
		}
	}

	void MainMenuBar::MenuFile(EditorContext& eCtx)
	{
		// @TODO : refactor clear code -> not clear registry, create new state
		if(ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Scene"))
			{
				// If current scene is not saved yet
				if (eCtx.sceneDirty) {/*open popup*/ }

				eCtx.currentState->GetRegistry().clear();
				eCtx.currentState->GetEntityMap().clear();
			}

			if (ImGui::MenuItem("Open Scene"))
			{
				//// If current scene is not saved yet, open popup
				//if (eCtx.sceneDirty) {}

				auto path = FileDialog::OpenFile("Open Scene");
				if (path)
				{
					std::string dir = path.value().string();

					eCtx.selectedEntity = entt::null;
					//eCtx.currentState->GetRegistry().clear();
					eCtx.currentState->GetEntityMap().clear();

					Serialization::LoadScene(eCtx.currentState->GetRegistry(),
						dir.c_str(),
						eCtx.currentState->GetEntityMap());
				}
			}

			ImGui::Separator();
			if (ImGui::MenuItem("Save"))
			{
				// if scene file is not exist, create new one
				//if(eCtx.currentScenePath)

				if (eCtx.sceneDirty)
				{
					Serialization::SaveScene(eCtx.currentState->GetRegistry(),
						"Resources/Engine/Assets/test.data");

				}
			}

			if (ImGui::MenuItem("Save As..."))
			{
				
				Serialization::SaveScene(eCtx.currentState->GetRegistry(),
					"Resources/Engine/Assets/test.data");
			}
			ImGui::EndMenu();
		}
	}
}