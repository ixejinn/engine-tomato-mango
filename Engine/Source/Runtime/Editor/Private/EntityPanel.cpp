#include "EntityPanel.h"

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <entt/entt.hpp>
#include "State/State.h"

#include "ECS/Components/Nametag.h"

namespace tomato
{
	void EntityPanel::Draw(EditorContext& editorCtx)
	{
		ImGui::SetNextWindowPos(ImVec2(800.f, 450.f), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(400, 220), ImGuiCond_Always);
		
		if (ImGui::Begin("Entity List", 0, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize))
		{
			auto view = editorCtx.currentState->GetRegistry().view<NametagComponent>();
			for (auto [e, tag] : view.each())
			{
				if (ImGui::Selectable(tag.name.c_str()))
				{
					editorCtx.selectedEntity = e;
				}
			}

		}



		ImGui::End();
	}
}