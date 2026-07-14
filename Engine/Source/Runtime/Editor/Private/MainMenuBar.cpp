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
#include "Resource/PathManager.h"

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

				eCtx.currentScenePath = "";
				eCtx.sceneDirty = true;
			}

			if (ImGui::MenuItem("Open Scene"))
			{
				//// If current scene is not saved yet, open popup
				//if (eCtx.sceneDirty) {}

				auto path = FileDialog::OpenFile("Open Scene");
				if (path)
				{
					eCtx.selectedEntity = entt::null;
					eCtx.currentState->GetEntityMap().clear();

					Serialization::LoadScene(eCtx.currentState->GetRegistry(),
						path.value().string().c_str(),
						eCtx.currentState->GetEntityMap());

					eCtx.currentScenePath = path.value();
					eCtx.sceneDirty = false;
				}
			}

			ImGui::Separator();
			if (ImGui::MenuItem("Save"))
			{
				// if scene file is not exist, create new one
				if (eCtx.currentScenePath.empty())
					SaveAs(eCtx);

				//if (eCtx.sceneDirty)
					Serialization::SaveScene(eCtx.currentState->GetRegistry(),
						eCtx.currentScenePath.string().c_str());

					eCtx.sceneDirty = false;
			}

			if (ImGui::MenuItem("Save As..."))
				SaveAs(eCtx);
			
			ImGui::EndMenu();
		}
	}

	void MainMenuBar::SaveAs(EditorContext& eCtx)
	{
		auto path = FileDialog::SaveFile(
			"Save Scene",
			"scene",
			"Scene Files (*.scene)\0*.scene\0""All Files (*.*)\0*.*\0",
			"Resources/Contents/Scenes");

		if (path)
		{
			Serialization::SaveScene(eCtx.currentState->GetRegistry(),
				path.value().string().c_str());

			eCtx.currentScenePath = path.value();
			eCtx.sceneDirty = false;
		}
	}
}