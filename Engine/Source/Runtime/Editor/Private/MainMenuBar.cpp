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

#include "Utils/Bitmask/BitmaskOperators.h"
#include "Utils/FileDialog.h"
#include "Resource/PathManager.h"
#include <iostream>
namespace tomato
{
	void MainMenuBar::Draw(EditorContext& eCtx, RunMode& mode)
	{
		ProcessShortcuts(eCtx);
		if (ImGui::BeginMainMenuBar())
		{
			MenuFile(eCtx);

			EditModeButton(eCtx, mode);

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
			if (ImGui::MenuItem("New Scene", "Ctrl+N"))
				NewScene(eCtx);

			if (ImGui::MenuItem("Open Scene", "Ctrl+O"))
				OpenScene(eCtx);

			ImGui::Separator();

			if (ImGui::MenuItem("Save", "Ctrl+S"))
				Save(eCtx);

			if (ImGui::MenuItem("Save As..."))
				SaveAs(eCtx);
			
			ImGui::EndMenu();
		}
	}

	void MainMenuBar::EditModeButton(EditorContext& eCtx, RunMode& mode)
	{
		float buttonX = ImGui::GetContentRegionAvail().x;
		ImGui::SetCursorPosX(buttonX / 2.f);

		static const char* playModeBtn = "▶";
		//Editor mode Start Button
		if (ImGui::Button(playModeBtn))
		{
			if (HasFlag(mode, RunMode::Game))
			{
				playModeBtn = "▶";
				mode = RunMode::Editor;
			}
			else
			{
				if (eCtx.sceneDirty)
					Save(eCtx);

				playModeBtn = "||";
				mode = RunMode::Game;
			}
		}
		//Editor mode Pause Button
		ImGui::BeginDisabled(HasFlag(mode, RunMode::Editor));
		if (ImGui::Button("■"))
		{
			playModeBtn = "▶";
			ReLoadScene(eCtx);
			mode = RunMode::Editor;
		}
		ImGui::EndDisabled();
	}

	void MainMenuBar::NewScene(EditorContext& eCtx)
	{
		// If current scene is not saved yet
		if (eCtx.sceneDirty)
		{
			pendingAction_ = PendingAction::NewScene;
			openNotSavedPopup = true;
		}

		else
		{
			Serialization::NewStateScene(eCtx.currentState);
			eCtx.sceneDirty = false;
			eCtx.currentScenePath = "";
			eCtx.currentSceneRuntimePath = "";
		}
	}

	void MainMenuBar::OpenScene(EditorContext& eCtx)
	{
		// If current scene is not saved yet, open popup
		if (eCtx.sceneDirty)
		{
			pendingAction_ = PendingAction::OpenScene;
			openNotSavedPopup = true;
		}
		else // From project root to runtime root
		{
			auto path = FileDialog::OpenFile("Open Scene", "Scene Files (*.scene)\0*.scene*\0\0", PathManager::ProjectResource() / "Scenes");
			if (!path.has_value())
				return;

			auto runtimePath = PathManager::ToRuntime(path.value());
			if (!runtimePath.empty())
			{
				FileUtils::CopyAsset(path.value(), runtimePath, std::filesystem::copy_options::overwrite_existing);
				Serialization::LoadStateScene(
					eCtx.currentState,
					runtimePath.string().c_str()
				);

				eCtx.currentScenePath = path.value();
				eCtx.currentSceneRuntimePath = runtimePath;
				eCtx.sceneDirty = false;

				std::cout << "Load Scene: " << eCtx.currentScenePath << '\n';
			}
		}
	}

	void MainMenuBar::Save(EditorContext& eCtx)
	{
		// if scene file is not exist, SaveAs
		if (eCtx.currentScenePath.empty())
			SaveAs(eCtx);

		else
		{
			Serialization::SaveScene(eCtx.currentState,
				eCtx.currentScenePath.string().c_str());
			FileUtils::CopyAsset(eCtx.currentScenePath, eCtx.currentSceneRuntimePath, std::filesystem::copy_options::overwrite_existing);
		}

		eCtx.sceneDirty = false;
	}

	void MainMenuBar::SaveAs(EditorContext& eCtx)
	{
		auto path = FileDialog::SaveFile(
			"Save Scene",
			"scene",
			"Scene Files (*.scene)\0*.scene\0""All Files (*.*)\0*.*\0",
			PathManager::ProjectResource() / "Scenes");

		if (path)
		{
			auto runtimePath = PathManager::ToRuntime(path.value());

			Serialization::SaveScene(eCtx.currentState,
				path.value().string().c_str());
			FileUtils::CopyAsset(path.value(), runtimePath, std::filesystem::copy_options::overwrite_existing);

			eCtx.currentScenePath = path.value();
			eCtx.currentSceneRuntimePath = runtimePath;
			eCtx.sceneDirty = false;
		}
	}

	void MainMenuBar::ReLoadScene(EditorContext& eCtx)
	{
		auto& path = eCtx.currentScenePath;
		if (!path.empty())
		{
			Serialization::LoadStateScene(
				eCtx.currentState,
				path.string().c_str()
			);
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
		if (saved)
			Save(eCtx);

		ImGui::CloseCurrentPopup();
		eCtx.sceneDirty = false;
		eCtx.currentScenePath = "";
		eCtx.currentSceneRuntimePath = "";

		switch (pendingAction_)
		{

			case PendingAction::NewScene:
				
				pendingAction_ = PendingAction::None;
				NewScene(eCtx);
				break;

			case PendingAction::OpenScene:

				pendingAction_ = PendingAction::None;
				OpenScene(eCtx);
				break;
		}
	}
	void MainMenuBar::ProcessShortcuts(EditorContext& eCtx)
	{
		ImGuiInputFlags shortcutFlags = ImGuiInputFlags_RouteGlobal | ImGuiInputFlags_RouteOverFocused;

		if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_N, shortcutFlags))
			NewScene(eCtx);
		
		if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_O, shortcutFlags))
			OpenScene(eCtx);

		if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S, shortcutFlags))
			Save(eCtx);

	}
}