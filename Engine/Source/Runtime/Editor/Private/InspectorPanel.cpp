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

		ImGui::SetNextWindowPos(ImVec2(1600.f, 300.f + height), ImGuiCond_Appearing, ImVec2(1.f, 1.f));
		ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);
		
		if (ImGui::Begin("Inspector", 0, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize))
		{
			auto& componentInfo = Serialization::ComponentRegistry::GetInstance().GetComponentInfo();
			for (const auto& comp : componentInfo)
			{
				if (!comp.Has(editorCtx.currentState->GetRegistry(), editorCtx.selectedEntity))
					continue;

				bool is_open = ImGui::CollapsingHeader(comp.name.c_str());
				if (is_open && comp.Draw)
				{
					comp.Draw(editorCtx, editorCtx.currentState->GetRegistry(), editorCtx.selectedEntity);
				}
			}
		}
		ImGui::End();
	}

}