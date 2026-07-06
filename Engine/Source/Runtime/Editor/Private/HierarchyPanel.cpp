#include "HierarchyPanel.h"

#include "Prefab/Prefab.h"
#include "Prefab/UIPrefab.h"

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

#include "ECS/Entity/Hierarchy.h"

#include <iostream>
namespace tomato
{
	void HierarchyPanel::Draw(EditorContext& editorCtx)
	{
		float width{ 400.f }, height{ 300.f };
		
		ImGui::SetNextWindowPos(ImVec2(1600.f, height), ImGuiCond_FirstUseEver, ImVec2(1.f, 1.f));
		ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_FirstUseEver);

		if (ImGui::Begin("Hierarchy", 0, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus))
		{
			MenuBar(editorCtx);
			ImGuiTreeNodeFlags tFlags = ImGuiTreeNodeFlags_DefaultOpen;
			if (ImGui::CollapsingHeader("Scene", tFlags))
			{
				int node_n = 0;

				auto view = editorCtx.currentState->GetRegistry().view<RootEntityTag, NametagComponent>();
				for (auto [e, tag] : view.each())
				{
					ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAllColumns | ImGuiTreeNodeFlags_DrawLinesFull | ImGuiTreeNodeFlags_DrawLinesToNodes | ImGuiTreeNodeFlags_OpenOnArrow;

					if (editorCtx.selectedEntity == e)
						flags |= ImGuiTreeNodeFlags_Selected;

					bool is_open = ImGui::TreeNodeEx((void*)(intptr_t)node_n, flags, tag.name.c_str(), node_n);

					if (ImGui::IsItemClicked())
						editorCtx.selectedEntity = e;

					if (ImGui::BeginPopupContextItem())
					{
						if(editorCtx.selectedEntity != e)
							editorCtx.selectedEntity = e;

						ShowMoreButton(editorCtx);
						
						ImGui::EndPopup();
					}
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

		auto* hierarchy = reg.try_get<HierarchyComponent>(e);
		if (!hierarchy)
			return;

		if (hierarchy->children.empty())
			return;

		for (auto& child : hierarchy->children)
		{
			auto& nameTag = reg.get<NametagComponent>(child);

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAllColumns | ImGuiTreeNodeFlags_DrawLinesFull | ImGuiTreeNodeFlags_DrawLinesToNodes | ImGuiTreeNodeFlags_OpenOnArrow;

			if (editorCtx.selectedEntity == child)
				flags |= ImGuiTreeNodeFlags_Selected;

			bool is_open = ImGui::TreeNodeEx(nameTag.name.c_str(), flags);

			if (ImGui::IsItemClicked())
				editorCtx.selectedEntity = child;

			if (ImGui::BeginPopupContextItem())
			{
				if (editorCtx.selectedEntity != child)
					editorCtx.selectedEntity = child;

				ShowMoreButton(editorCtx);

				ImGui::EndPopup();
			}

			if (is_open)
			{
				Traverse(editorCtx, child);
				ImGui::TreePop();
			}
		}
	}
	void HierarchyPanel::CreateAndSetHierarchyEntity(EditorContext& editorCtx, entt::entity e, bool hierarchy)
	{
		if (hierarchy)
			SetHierarchy(editorCtx.currentState->GetRegistry(), editorCtx.selectedEntity, e);
	}

	void HierarchyPanel::MenuBar(EditorContext& editorCtx)
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Add"))
			{
				AddItems(editorCtx, false);

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
	}

	void HierarchyPanel::AddItems(EditorContext& editorCtx, bool isPopup)
	{
		entt::registry& reg = editorCtx.currentState->GetRegistry();
		auto selected = editorCtx.selectedEntity;

		if (ImGui::MenuItem("Create Empty"))
			 CreateAndSetHierarchyEntity(editorCtx, selected = Prefab::CreateEmpty(reg), isPopup);
		

		if (ImGui::BeginMenu("Game Object"))
		{
			if (ImGui::BeginMenu("Static"))
			{
				if (ImGui::MenuItem("Cube"))
					CreateAndSetHierarchyEntity(editorCtx, selected = Prefab::CreateStaticObject(reg), isPopup);

				if (ImGui::MenuItem("Sphere"))
					CreateAndSetHierarchyEntity(editorCtx, selected = Prefab::CreateStaticObject(reg, Prefab::Sphere), isPopup);

				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Character"))
				CreateAndSetHierarchyEntity(editorCtx, selected = Prefab::CreateCharacter(reg), isPopup);

			if (ImGui::MenuItem("Camera"))
			{
				auto view = reg.view<MainCameraTag>();
				CreateAndSetHierarchyEntity(
					editorCtx,
					selected = Prefab::CreateCamera(reg, view.empty() == true ? true : false),
					isPopup);
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("UI"))
		{
			if (ImGui::MenuItem("Canvas"))
				CreateAndSetHierarchyEntity(editorCtx, selected = UIPrefab::CreateCanvas(reg), isPopup);

			if (ImGui::MenuItem("Button"))
				CreateAndSetHierarchyEntity(editorCtx, selected = UIPrefab::CreateButton(reg), isPopup);				

			if (ImGui::MenuItem("Text"))
				CreateAndSetHierarchyEntity(editorCtx, selected = UIPrefab::CreateText(reg), isPopup);

			if (ImGui::MenuItem("Image"))
				CreateAndSetHierarchyEntity(editorCtx, selected = UIPrefab::CreateImage(reg), isPopup);

			ImGui::EndMenu();
		}

		editorCtx.selectedEntity = selected;
	}

	void HierarchyPanel::ShowMoreButton(EditorContext& editorCtx)
	{
		int t_entitynum = (int)editorCtx.selectedEntity;

		auto& reg = editorCtx.currentState->GetRegistry();
		if (ImGui::MenuItem("Delete"))
		{
			DestroyHierarchyEntity(reg, editorCtx.selectedEntity);
			editorCtx.selectedEntity = entt::null;

			std::cout << "delete entity " << t_entitynum << '\n';
		}

		ImGui::Separator();

		AddItems(editorCtx, true);
	}
}