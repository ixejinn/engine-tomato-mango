#include "InspectorPanel.h"

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <entt/entt.hpp>
#include "State/State.h"

#include "Serialization/ComponentRegistry.h"

#include "ECS/Components/Nametag.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/Camera.h"

namespace tomato
{
	void InspectorPanel::Draw(EditorContext& editorCtx)
	{
		if (editorCtx.selectedEntity == entt::null)
			return;

		float width{ 300.f }, height{ 600.f };

		ImGui::SetNextWindowPos(ImVec2(1600.f, 300.f + height), ImGuiCond_FirstUseEver, ImVec2(1.f, 1.f));
		ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_FirstUseEver);
		
		if (ImGui::Begin("Inspector", 0, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize))
		{
			MenuBar(editorCtx);
			auto& componentInfo = Serialization::ComponentRegistry::GetInstance().GetComponentInfo();
			for (const auto& comp : componentInfo)
			{
				if (!comp.Has(editorCtx.currentState->GetRegistry(), editorCtx.selectedEntity))
					continue;

				bool is_open = ImGui::CollapsingHeader(comp.name.c_str());
				if (is_open && comp.editor.Draw)
				{
					comp.editor.Draw(editorCtx, editorCtx.currentState->GetRegistry(), editorCtx.selectedEntity);
				}
			}
		}
		ImGui::End();
	}

	void InspectorPanel::MenuBar(EditorContext& editorCtx)
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Add"))
			{
				if (ImGui::BeginMenu("Component"))
				{
					for (auto info : Serialization::componentCategoryMetas)
					{
						if (info.category == Serialization::ComponentCategory::Tag) continue;
						if (ImGui::BeginMenu(info.name))
						{
							ShowAddComponent(editorCtx, info.category);
							ImGui::EndMenu();
						}
					}
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Tag"))
				{
					ShowAddComponent(editorCtx, Serialization::ComponentCategory::Tag);
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
	}

	void InspectorPanel::ShowAddComponent(EditorContext& editorCtx, Serialization::ComponentCategory category)
	{
		auto& reg = editorCtx.currentState->GetRegistry();

		bool enabled = editorCtx.selectedEntity != entt::null ? true : false;

		auto& componentInfo = Serialization::ComponentRegistry::GetInstance().GetComponentInfo();
		for (const auto& comp : componentInfo)
		{
			if (comp.category == category)
			{
				if (ImGui::MenuItem(comp.name.c_str(), NULL, false, enabled))
					comp.editor.Add(reg, editorCtx.selectedEntity);
			}
		}
	}
}