#include "InspectorPanel.h"

#include "Resource/AssetRegistry.h"
#include "Resource/PathManager.h"
#include "Resource/Render/Texture.h"

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"

#include <entt/entt.hpp>
#include <fstream>
#include "State/State.h"

#include "Serialization/ComponentRegistry.h"

#include "ECS/Components/Nametag.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/Camera.h"
#include "ECS/Components/EditorTag.h"

#include "Utils/FileDialog.h"
#include "Utils/Bitmask/BitmaskOperators.h"

namespace tomato
{
	InspectorPanel::InspectorPanel(float width, float height, float x, float y) : EditorPanel(width, height, x, y)
	{
		LoadResources();
	}

	void InspectorPanel::Draw(EditorContext& editorCtx)
	{
		if (editorCtx.selectedEntity == entt::null)
			return;

		if (editorCtx.currentState->GetRegistry().all_of<NoInspector>(editorCtx.selectedEntity))
			return;

		ImGui::SetNextWindowPos(ImVec2(posX_, posY_), ImGuiCond_FirstUseEver, ImVec2(1.f, 1.f));
		ImGui::SetNextWindowSize(ImVec2(width_, height_), ImGuiCond_FirstUseEver);
		
		if (ImGui::Begin("Inspector", 0, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize))
		{
			SetPos({ ImGui::GetWindowPos().x, ImGui::GetWindowPos().y });

			MenuBar(editorCtx);
			ShowEntityUID(editorCtx);

			auto& componentInfo = Serialization::ComponentRegistry::GetInstance().GetComponentInfo();
			for (const auto& comp : componentInfo)
			{
				if (!comp.Has(editorCtx.currentState->GetRegistry(), editorCtx.selectedEntity))
					continue;

				if (HasFlag(comp.flags, Serialization::ComponentFlags::Hidden))
					continue;

				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_AllowOverlap;
				if(!comp.editor.Draw)
					flags |= ImGuiTreeNodeFlags_Leaf;

				bool is_open = ImGui::CollapsingHeader(comp.name.c_str(), flags);

				MoreButton(editorCtx, comp, is_open);

				if (is_open && comp.editor.Draw)
				{
					if (comp.editor.Draw(editorCtx,
						editorCtx.currentState->GetRegistry(),
						editorCtx.selectedEntity))
						editorCtx.sceneDirty = true;
				}
			}
		}
		ImGui::End();
	}

	void InspectorPanel::LoadResources()
	{
		more_vert =
			AssetRegistry<Texture>::GetInstance().
			Get(GetAssetID(PathManager::RuntimeIcon("more_vert.png").string().c_str()))->GetTexture();
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
			if (ImGui::InputText("##input text", &nametag.name, ImGuiInputTextFlags_ElideLeft))
				editorCtx.sceneDirty = true;
			//ImGui::Text("%s", nametag.name.c_str());

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
			//if (HasFlag<Serialization::ComponentFlags>(comp.flags, Serialization::ComponentFlags::Hidden))
			if (HasFlag(comp.flags, Serialization::ComponentFlags::Hidden))
				continue;

			if (comp.category == category)
			{
				if (ImGui::MenuItem(comp.name.c_str(), NULL, false, enabled))
				{
					comp.editor.Add(reg, editorCtx.selectedEntity);
					editorCtx.sceneDirty = true;
				}
			}
		}
	}

	void InspectorPanel::MoreButton(EditorContext& editorCtx, const Serialization::ComponentInfo& comp, bool& isOpen)
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
				//if (!HasFlag<Serialization::ComponentFlags>(comp.flags, Serialization::ComponentFlags::Essential))
				if (!HasFlag(comp.flags, Serialization::ComponentFlags::Essential))
				{
					comp.editor.Remove(editorCtx.currentState->GetRegistry(), editorCtx.selectedEntity);
					isOpen = false;
					editorCtx.sceneDirty = true;
				}
			}

			if (comp.category == Serialization::ComponentCategory::Particle)
				SaveParticleButton(editorCtx, comp);

			ImGui::EndPopup();
		}
		ImGui::PopID();
	}

	void InspectorPanel::SaveParticleButton(EditorContext& editorCtx, const Serialization::ComponentInfo& comp)
	{
		if (ImGui::MenuItem("Save"))
		{
			auto path = FileDialog::SaveFile(
				"Save Particle",
				"ptc",
				"Particle Files (*.ptc)\0*.ptc\0\0",
				PathManager::ProjectResource() / "Particle");

			if (path)
			{
				auto runtimePath = PathManager::ToRuntime(path.value());
				
				Serialization::SaveParticle(path.value(), editorCtx.currentState->GetRegistry(), editorCtx.selectedEntity);
				FileUtils::CopyAsset(path.value(), runtimePath, std::filesystem::copy_options::overwrite_existing);
			}
		}
	}

}