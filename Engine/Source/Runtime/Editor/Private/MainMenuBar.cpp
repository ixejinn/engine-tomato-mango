#include "MainMenuBar.h"

#include <string>
#include <entt/entt.hpp>

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <implot/implot.h>
#include <implot/implot_internal.h>

#include "EditorPanel.h"
#include "State/State.h"
#include "State/StateRegistry.h"

#include "Serialization/ComponentSerializer.h"

#include "Utils/Bitmask/BitmaskOperators.h"
#include "Utils/FileDialog.h"
#include "Resource/PathManager.h"
#include <iostream>

#include "Event/EventDispatcher.h"
#include "ECS/SystemFramework/ChangeRunModeEvent.h"

#include "Profiler/Profiler.h"
#include "Profiler/ExecutionTime.h"

namespace tomato
{
	void MainMenuBar::Draw(EditorContext& eCtx, RunMode& mode)
	{
		ProcessShortcuts(eCtx);
		if (ImGui::BeginMainMenuBar())
		{
			MenuFile(eCtx);
			MenuTools(eCtx);

			EditModeButton(eCtx, mode);

			ImGui::EndMainMenuBar();
		}

		if (openNotSavedPopup)
		{
			ImGui::OpenPopup("Not Saved");
			openNotSavedPopup = false;
		}
		OpenPopupModal(eCtx);

		if (showCPUProfiler)
			ShowCPUProfilerWindow(eCtx);
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

	void MainMenuBar::MenuTools(EditorContext&)
	{
		if (ImGui::BeginMenu("Tools"))
		{
			if (ImGui::MenuItem("CPU Profiler", NULL, &showCPUProfiler))
			{
				auto& profiler = Profiler::GetInstance();
				profiler.SetActive(showCPUProfiler);

				if (showCPUProfiler)
					profiler.Start();
				else
					profiler.End();
			}

			ImGui::EndMenu();
		}
	}

	void MainMenuBar::EditModeButton(EditorContext& eCtx, RunMode& mode)
	{
        RunMode preMode = mode;

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

        if (preMode != mode)
            EventDispatcher::GetInstance().Enqueue(ChangeRunModeEvent{mode});
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

	void MainMenuBar::ShowCPUProfilerWindow(EditorContext&)
	{
		auto& profiler = Profiler::GetInstance();
		profiler.Update();

		size_t currFrame = profiler.GetFrameCnt();
		size_t offset = currFrame - PROFILER_SAMPLE_COUNT;
		if (currFrame < PROFILER_SAMPLE_COUNT)
			offset = 0;

		// Create ImGui window
		ImGui::SetNextWindowPos(ImVec2(0.f, ImGui::GetFrameHeight()), ImGuiCond_Once, ImVec2(0.f, 0.f));
		ImGui::SetNextWindowSize(ImVec2(600.f, 150.f), ImGuiCond_Once);

		// Draw CPU profiler graph on the window
		if (ImGui::Begin("CPU Profiler", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar))
		{
			auto& registry = profiler.GetRegistry();

			if (ImPlot::BeginPlot("##CPU Profiler Graph", ImVec2(-1, 115), ImPlotFlags_Crosshairs))
			{
				ImPlot::SetupAxes(nullptr, "us", ImPlotAxisFlags_NoTickLabels);

				ImPlot::SetupAxisLimitsConstraints(ImAxis_Y1, 0, INFINITY);
				ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 20000, ImPlotCond_Once);
				ImPlot::SetupAxisLimits(ImAxis_X1, 0, PROFILER_SAMPLE_COUNT, ImPlotCond_Always);

				auto view = registry.view<ExecutionTimeHistoryComponent>();
				for (auto [e, exeHistory] : view.each())
				{
					ImPlotSpec spec;
					spec.Offset = offset;

					const auto* data = exeHistory.data.GetRawArray().data();

					if (registry.all_of<TotalFrameTag>(e))
					{
						spec.FillAlpha = 0.35f;
						ImPlot::PlotShaded(exeHistory.name.data(), data, PROFILER_SAMPLE_COUNT, INFINITY, 1, 0, spec);
					}
					ImPlot::PlotLine(exeHistory.name.data(), data, PROFILER_SAMPLE_COUNT, 1, 0, spec);
				}

				ImPlot::EndPlot();
			}

			ImGui::End();
		}
	}
}
