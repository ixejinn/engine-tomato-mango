#include "HierarchyPanel.h"

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <entt/entt.hpp>
#include "State/State.h"

#include "ECS/Components/Nametag.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/Camera.h"

namespace tomato
{
	void HierarchyPanel::Draw(EditorContext& editorCtx)
	{
		float width{ 400.f }, height{ 300.f };
		
		ImGui::SetNextWindowPos(ImVec2(1600.f, height), ImGuiCond_Appearing, ImVec2(1.f, 1.f));
		ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);

		if (ImGui::Begin("Hierarchy", 0, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize))
		{
			ImGuiTreeNodeFlags tFlags = ImGuiTreeNodeFlags_DefaultOpen;
			if (ImGui::CollapsingHeader("Scene", tFlags))
			{
				int node_n = 0;

				auto view = editorCtx.currentState->GetRegistry().view<RootEntityTag, NametagComponent>();
				for (auto [e, tag] : view.each())
				{
					ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

					if (editorCtx.selectedEntity == e)
						flags |= ImGuiTreeNodeFlags_Selected;

					bool is_open = ImGui::TreeNodeEx((void*)(intptr_t)node_n, flags, tag.name.c_str(), node_n);

					if (ImGui::IsItemClicked())
						editorCtx.selectedEntity = e;

					if (is_open)
					{
						Traverse(editorCtx, e);

						ImGui::TreePop();
					}

					node_n++;
				}
			}
		}
		ImGui::End();
	}

	void HierarchyPanel::Traverse(EditorContext& editorCtx, entt::entity e)
	{
		auto& reg = editorCtx.currentState->GetRegistry();

		if (reg.all_of<MainCameraTag>(e))
			return;

		auto& hierarchy = reg.get<HierarchyComponent>(e);
		if (hierarchy.children.empty())
			return;

		for (auto& child : hierarchy.children)
		{
			auto& nameTag = reg.get<NametagComponent>(child);

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

			if (editorCtx.selectedEntity == child)
				flags |= ImGuiTreeNodeFlags_Selected;

			bool is_open = ImGui::TreeNodeEx(nameTag.name.c_str(), flags);

			if (ImGui::IsItemClicked())
				editorCtx.selectedEntity = child;

			if (is_open)
			{
				Traverse(editorCtx, child);
				ImGui::TreePop();
			}
		}
	}
}