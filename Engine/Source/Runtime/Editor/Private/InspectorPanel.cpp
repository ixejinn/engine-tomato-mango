#include "InspectorPanel.h"

#include "Resource/AssetRegistry.h"
#include "Resource/Render/Texture.h"

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
	InspectorPanel::InspectorPanel(bool open) : EditorPanel(open)
	{
		LoadResources();
	}

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
			ShowEntityUID(editorCtx);

			auto& componentInfo = Serialization::ComponentRegistry::GetInstance().GetComponentInfo();
			for (const auto& comp : componentInfo)
			{
				if (!comp.Has(editorCtx.currentState->GetRegistry(), editorCtx.selectedEntity))
					continue;

				if (HasFlag<Serialization::ComponentFlags>(comp.flags, Serialization::ComponentFlags::Hidden))
					continue;

				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_AllowOverlap;
				if(!comp.editor.Draw)
					flags |= ImGuiTreeNodeFlags_Leaf;

				bool is_open = ImGui::CollapsingHeader(comp.name.c_str(), flags);

				MoreButton(editorCtx, comp);

				if (is_open && comp.editor.Draw)
					comp.editor.Draw(editorCtx, editorCtx.currentState->GetRegistry(), editorCtx.selectedEntity);
			}
		}
		ImGui::End();
	}

	void InspectorPanel::LoadResources()
	{
		more_vert =
			AssetRegistry<Texture>::GetInstance().
			Get(GetAssetID("Resources/Engine/Assets/img/more_vert.png"))->GetTexture();
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

	void InspectorPanel::ShowEntityUID(EditorContext& editorCtx)
	{
		auto& nametag = editorCtx.currentState->GetRegistry().get<NametagComponent>(editorCtx.selectedEntity);
		if(ImGui::BeginTable("EntityInformation", 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInner))
		{
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("Name").x + 20.f);
			ImGui::TableSetupColumn("Entity", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("Entity").x);
			ImGui::TableSetupColumn("UUID");
			ImGui::TableHeadersRow();

			ImGui::PushStyleVarY(ImGuiStyleVar_CellPadding, 5.0f);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("%s", nametag.name.c_str());

			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%d", entt::to_entity(editorCtx.selectedEntity));

			ImGui::TableSetColumnIndex(2);
			ImGui::Text("%llu", nametag.id);

			ImGui::PopStyleVar();

			ImGui::EndTable();
		}
		ImGui::NewLine();
	}

	void InspectorPanel::ShowAddComponent(EditorContext& editorCtx, Serialization::ComponentCategory category)
	{
		auto& reg = editorCtx.currentState->GetRegistry();

		bool enabled = editorCtx.selectedEntity != entt::null ? true : false;

		auto& componentInfo = Serialization::ComponentRegistry::GetInstance().GetComponentInfo();
		for (const auto& comp : componentInfo)
		{
			if (HasFlag<Serialization::ComponentFlags>(comp.flags, Serialization::ComponentFlags::Hidden))
				continue;

			if (comp.category == category)
			{
				if (ImGui::MenuItem(comp.name.c_str(), NULL, false, enabled))
					comp.editor.Add(reg, editorCtx.selectedEntity);
			}
		}
	}

	void InspectorPanel::MoreButton(EditorContext& editorCtx, const Serialization::ComponentInfo& comp)
	{
		ImGui::SameLine();

		auto w = ImGui::GetContentRegionMax().x - 18.f;
		ImGui::SetCursorPosX(w);
		
		ImGui::PushID(comp.name.c_str());
		if (ImGui::ImageButton("##more_vert", more_vert, ImVec2(14.f, 14.f)))
			ImGui::OpenPopup("inspector more popup");

		if (ImGui::BeginPopup("inspector more popup"))
		{
			if (ImGui::MenuItem("Remove Component"))
			{
				if (!HasFlag<Serialization::ComponentFlags>(comp.flags, Serialization::ComponentFlags::Essential))
					comp.editor.Remove(editorCtx.currentState->GetRegistry(), editorCtx.selectedEntity);
			}

			ImGui::EndPopup();
		}
		ImGui::PopID();
	}

}