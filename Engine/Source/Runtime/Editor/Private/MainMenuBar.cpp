#include "MainMenuBar.h"

#include <string>
#include <entt/entt.hpp>

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "EditorPanel.h"
#include "State/State.h"
#include "State/StateRegistry.h"

#include "Serialization/ComponentSerializer.h"

#include "Utils/FileDialog.h"
#include "Resource/PathManager.h"
#include <iostream>
namespace tomato
{
	void MainMenuBar::Draw(EditorContext& eCtx)
	{
		if (ImGui::BeginMainMenuBar())
		{
			MenuFile(eCtx);
			ImGui::EndMainMenuBar();
		}
		if (openNotSavedPopup)
		{
			ImGui::OpenPopup("Not Saved");
			openNotSavedPopup = false;
		}
		OpenPopupModal(eCtx);
	}

	void MainMenuBar::MenuFile(EditorContext& eCtx)
	{
		if(ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Scene"))
			{
				// If current scene is not saved yet
				if (eCtx.sceneDirty)
				{
					pendingAction_ = PendingAction::NewScene;
					openNotSavedPopup = true;
				}
				
				else
					NewScene(eCtx);
			}

			if (ImGui::MenuItem("Open Scene"))
			{
				// If current scene is not saved yet, open popup
				if (eCtx.sceneDirty)
				{
					pendingAction_ = PendingAction::OpenScene;
					openNotSavedPopup = true;
				}
				else
					OpenScene(eCtx);
			}

			ImGui::Separator();
			if (ImGui::MenuItem("Save"))
				Save(eCtx);

			if (ImGui::MenuItem("Save As..."))
				SaveAs(eCtx);
			
			ImGui::EndMenu();
		}
	}

	void MainMenuBar::NewScene(EditorContext& eCtx)
	{
		/*eCtx.currentState->GetRegistry().clear();
		eCtx.currentState->GetEntityMap().clear();
		eCtx.selectedEntity = entt::null;

		eCtx.currentScenePath = "";
		eCtx.sceneDirty = false;*/

		Serialization::NewStateScene(eCtx.currentState->GetEngine(), eCtx.currentState);

	}

	void MainMenuBar::OpenScene(EditorContext& eCtx)
	{
		auto path = FileDialog::OpenFile("Open Scene");
		if (path)
		{
			Serialization::LoadStateScene(
				eCtx.currentState->GetEngine(),
				eCtx.currentState,
				path.value().string().c_str()
			);
		}
	}

	void MainMenuBar::Save(EditorContext& eCtx)
	{
		// if scene file is not exist, SaveAs
		if (eCtx.currentScenePath.empty())
			SaveAs(eCtx);

		else
			Serialization::SaveScene(eCtx.currentState,
				eCtx.currentScenePath.string().c_str());

		eCtx.sceneDirty = false;
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
			Serialization::SaveScene(eCtx.currentState,
				path.value().string().c_str());

			eCtx.currentScenePath = path.value();
			eCtx.sceneDirty = false;
		}
	}

	void MainMenuBar::OpenPopupModal(EditorContext& eCtx)
	{
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal("Not Saved", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Do you want to save your changes?\n%s",
				eCtx.currentScenePath.empty() == true ? "" : eCtx.currentScenePath.string().c_str());
			ImGui::Separator();

			ImGui::SetItemDefaultFocus();
			if (ImGui::Button("Save", ImVec2(120, 0)))
				ExecutePendingAction(eCtx, true);

			ImGui::SameLine();
			if (ImGui::Button("Don't Save", ImVec2(120, 0)))
				ExecutePendingAction(eCtx, false);
			
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0)))
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}
	}

	void MainMenuBar::ExecutePendingAction(EditorContext& eCtx, bool saved)
	{
		switch (pendingAction_)
		{
			case PendingAction::NewScene:
				if (saved)
					Save(eCtx);
				
				ImGui::CloseCurrentPopup();
				NewScene(eCtx);
				pendingAction_ = PendingAction::None;
				break;

			case PendingAction::OpenScene:
				if(saved)
					Save(eCtx);

				ImGui::CloseCurrentPopup();
				OpenScene(eCtx);
				pendingAction_ = PendingAction::None;
				break;
		}
	}
}