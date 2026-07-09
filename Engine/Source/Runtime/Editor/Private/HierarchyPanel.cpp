#include "HierarchyPanel.h"

#include "Prefab/Prefab.h"
#include "Prefab/UIPrefab.h"
#include "Prefab/EntityUtils.h"

#include "Resource/AssetRegistry.h"

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <entt/entt.hpp>
#include "State/State.h"

#include "Serialization/ComponentRegistry.h"

#include "ECS/Components/Nametag.h"
#include "ECS/Components/Visibility.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/Camera.h"

#include "ECS/Entity/Hierarchy.h"
#include <iostream>
namespace tomato
{
	HierarchyPanel::HierarchyPanel(bool open) : EditorPanel(open)
	{
		LoadResources();
	}

	void HierarchyPanel::LoadResources()
	{
		icon_visibility[0] =
			AssetRegistry<Texture>::GetInstance().
			Get(GetAssetID("Resources/Engine/Assets/img/visibility_off.png"))->GetTexture();

		icon_visibility[1] =
			AssetRegistry<Texture>::GetInstance().
			Get(GetAssetID("Resources/Engine/Assets/img/visibility_on.png"))->GetTexture();
	}

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

				auto view = editorCtx.currentState->GetRegistry().view<RootEntityTag>();

				for (auto e : view)
					DrawEntity(editorCtx, e);

				ImGui::InvisibleButton("HierarchyBackground", ImGui::GetContentRegionAvail());
				DragDropTargetBackground(editorCtx);
			}
		}
		ImGui::End();
	}

	void HierarchyPanel::DrawEntity(EditorContext& editorCtx, entt::entity e)
	{
		auto& r = editorCtx.currentState->GetRegistry();
		auto* hierarchy = r.try_get<HierarchyComponent>(e);

		bool opened = DrawRow(editorCtx, e);

		if (!opened)
			return;

		if(hierarchy)
		{
			for (auto child : hierarchy->children)
				DrawEntity(editorCtx, child);
		}

		ImGui::TreePop();
	}

	bool HierarchyPanel::DrawRow(EditorContext& editorCtx, entt::entity e)
	{
		auto& r = editorCtx.currentState->GetRegistry();

		auto& tag = r.get<NametagComponent>(e);
		VisibleButton(editorCtx, e, tag.name);
		ImGui::SameLine();
		
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow; // | ImGuiTreeNodeFlags_DrawLinesToNodes | ImGuiTreeNodeFlags_DrawLinesFull;
		
		auto* hierarchy = r.try_get<HierarchyComponent>(e);
		if (!hierarchy || hierarchy->children.empty())
			flags |= ImGuiTreeNodeFlags_Leaf;

		if (editorCtx.selectedEntity == e)
			flags |= ImGuiTreeNodeFlags_Selected;

		bool is_open = ImGui::TreeNodeEx((void*)(entt::id_type)e, flags, tag.name.c_str());
		
		DragDropSource(editorCtx, e);
		DragDropTarget(editorCtx, e);

		if (ImGui::IsItemClicked())
			editorCtx.selectedEntity = e;

		if (ImGui::BeginPopupContextItem())
		{
			if (editorCtx.selectedEntity != e)
				editorCtx.selectedEntity = e;

			ShowMoreButton(editorCtx);

			ImGui::EndPopup();
		}

		return is_open;
	}

	void HierarchyPanel::DragDropSource(EditorContext& editorCtx, entt::entity e)
	{
		if (ImGui::BeginDragDropSource())
		{
			entt::entity entity = e;
			ImGui::SetDragDropPayload("HIERARCHY_ENTITY", &entity, sizeof(entity));

			auto& tag = editorCtx.currentState->GetRegistry().get<NametagComponent>(e);
			ImGui::Text(tag.name.c_str());

			ImGui::EndDragDropSource();
		}
	}

	void HierarchyPanel::DragDropTarget(EditorContext& editorCtx, entt::entity e)
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_ENTITY"))
			{
				IM_ASSERT(payload->DataSize == sizeof(entt::id_type));
				auto dragged = *(const entt::entity*)payload->Data;

				SetHierarchy(editorCtx.currentState->GetRegistry(), e, dragged);
			}

			ImGui::EndDragDropTarget();
		}
	}

	void HierarchyPanel::DragDropTargetBackground(EditorContext& editorCtx)
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_ENTITY"))
			{
				IM_ASSERT(payload->DataSize == sizeof(entt::id_type));
				auto dragged = *(const entt::entity*)payload->Data;

				SetHierarchy(editorCtx.currentState->GetRegistry(), entt::null, dragged);
			}

			ImGui::EndDragDropTarget();
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

	void HierarchyPanel::VisibleButton(EditorContext& editorCtx, entt::entity e, std::string& str)
	{
		auto& reg = editorCtx.currentState->GetRegistry();

		std::string label = "visible" + str;

		auto* visibility = reg.try_get<VisibilityComponent>(e);
		if (ImGui::ImageButton(label.c_str(), icon_visibility[visibility->visible && visibility->inheritedVisible], ImVec2(15.f, 15.f)))
			ToggleVisible(reg, e);
	}

	void HierarchyPanel::ToggleVisible(entt::registry& reg, entt::entity e)
	{
		auto& visibility = reg.get<VisibilityComponent>(e);
		visibility.visible = !visibility.visible;

		UpdateInheritedVisibility(reg, e);
	}

	void HierarchyPanel::UpdateInheritedVisibility(entt::registry& reg, entt::entity e)
	{
		auto& visibility = reg.get<VisibilityComponent>(e);
		
		bool currentVisible = visibility.visible && visibility.inheritedVisible;

		auto* hierarchy = reg.try_get<HierarchyComponent>(e);
		if (!hierarchy) return;

		for (auto& child : hierarchy->children)
		{
			auto& cVisibility = reg.get<VisibilityComponent>(child);
			cVisibility.inheritedVisible = currentVisible;

			UpdateInheritedVisibility(reg, child);
		}
	}

	void SetVisible(entt::registry& reg, entt::entity e, bool parentVisibility)
	{
		auto* visibility = reg.try_get<VisibilityComponent>(e);
		if (visibility)
		{
			visibility->visible = !visibility->visible;
			visibility->inheritedVisible = parentVisibility;

			auto* hierarchy = reg.try_get<HierarchyComponent>(e);
			if (hierarchy)
			{
				for (auto& child : hierarchy->children)
					SetVisible(reg, child, visibility->inheritedVisible);
			}
		}
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