#include "InspectorDraw.h"

#include <limits>
#include <entt/entt.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "EditorPanel.h"
#include "Resource/AssetRegistry.h"
#include "Resource/Render/Mesh.h"
#include "Resource/Render/Shader.h"
#include "Resource/Render/Texture.h"
#include "Resource/Render/Font.h"

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "ECS/Components/Camera.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Rigidbody.h"
#include "ECS/Components/Collision.h"
#include "ECS/Components/Render.h"
#include "ECS/Components/UI.h"
#include "ECS/Components/UIEvents.h"
#include "ECS/Components/Text.h"
#include "ECS/Components/Nametag.h"
#include "ECS/Components/Hierarchy.h"

#include "ECS/Entity/Hierarchy.h"
#include "Prefab/EntityUtils.h"

namespace tomato
{
	void DrawCameraInspcetor(EditorContext& eCtx, entt::registry& reg, CameraComponent& camera)
	{
		ImGui::SeparatorText("Mode");

		if (ImGui::RadioButton("Perspective", camera.mode == ProjectionMode::Perspective))
		{
			camera.mode = ProjectionMode::Perspective;
			camera.dirty = true;
		}

		ImGui::SameLine();

		if (ImGui::RadioButton("Orthogonal", camera.mode == ProjectionMode::Orthogonal))
		{
			camera.mode = ProjectionMode::Orthogonal;
			camera.dirty = true;
		}

		float fov{ camera.degree }, near{ camera.zNear }, far{ camera.zFar };
		if (camera.mode == ProjectionMode::Perspective)
		{
			if(ImGui::SliderFloat("Field Of View", &fov, 1.f, 179.f, "%.2f"))
				camera.dirty = true;
		}
			
		if(ImGui::DragFloat("Near Clip", &near, 0.1f, 0.01f, 100.f, "%.2f")) camera.dirty = true;
		if(ImGui::DragFloat("Far Clip", &far, 0.1f, near, 10000.f, "%.2f")) camera.dirty = true;

		camera.degree = std::clamp(fov, 1.0f, 179.0f);
		camera.zNear = std::max(0.01f, near);
		camera.zFar = std::max(camera.zNear + 0.01f, far);

		ImGui::NewLine();
	}


	void DrawTransformInspector(EditorContext& eCtx, entt::registry& reg, TransformComponent& transform)
	{
		glm::vec3 pos = transform.GetLocalPosition();
		float posVec3[3] = { pos.x, pos.y, pos.z };
		if (DrawVec3Control("Position", posVec3))
			transform.SetPosition(posVec3[0], posVec3[1], posVec3[2]);

		glm::vec3 degree = transform.GetLocalEulerDegree();
		float degreeVec3[3] = { degree.x, degree.y, degree.z };
		if (DrawVec3Control("Degree", degreeVec3))
			transform.SetEulerDegree(degreeVec3[0], degreeVec3[1], degreeVec3[2]);

		glm::vec3 scale = transform.GetLocalScale();
		float scaleVec3[3] = { scale.x, scale.y, scale.z };
		if (DrawVec3Control("Scale", scaleVec3))
			transform.SetScale(scaleVec3[0], scaleVec3[1], scaleVec3[2]);

		ImGui::NewLine();
	}

	void DrawMovementInspector(EditorContext& eCtx, entt::registry& reg, MovementComponent& movement)
	{
		ImGui::SeparatorText("Speed");
		ImGui::DragFloat("Speed", &movement.horizontalSpeed, 1.0f, 0.f, 100.f, "%.2f");
		ImGui::NewLine();
	}

	void DrawVelocityInspector(EditorContext& eCtx, entt::registry& reg, VelocityComponent& vel)
	{
		float velVec3[3] = { vel.velocity.x, vel.velocity.y, vel.velocity.z };

		ImGuiSliderFlags flags = ImGuiSliderFlags_NoInput;
		if (DrawVec3Control("Velocity", velVec3, flags))
			vel.velocity = glm::vec3(velVec3[0], velVec3[1], velVec3[2]);
		ImGui::NewLine();
	}

	void DrawColliderInspector(EditorContext& eCtx, entt::registry& reg, ColliderComponent& collider)
	{
		//@TODO : Add EnumTraits
		ImGui::SeparatorText("Layer");
		const char* layPreview = "Default";
		if (ImGui::BeginCombo("##Layer", layPreview))
		{
			if (ImGui::Selectable("Default", collider.layer == CollisionLayer::Default))
				collider.layer = CollisionLayer::Default;

			ImGui::EndCombo();
		}

		ImGui::SeparatorText("Type");
		const char* typePreview = ColliderTypeMetas[(uint8_t)collider.type].name;
		if (ImGui::BeginCombo("##Type", typePreview))
		{
			for (const auto& info : ColliderTypeMetas)
			{
				if (ImGui::Selectable(info.name, collider.type == info.type))
				{
					collider.type = info.type;
					collider.aabbDirty = true;
				}
			}
			ImGui::EndCombo();
		}

		ImGui::SeparatorText("Trigger");
		ImGui::Text("Is Trigger"); ImGui::SameLine();
		ImGui::Checkbox("##Trigger", &collider.isTrigger);

		ImGui::NewLine();
	}

	void DrawRenderInspector(EditorContext& eCtx, entt::registry& reg, RenderComponent& render)
	{
		ImGui::SeparatorText("Color");
		ImGui::ColorEdit4("##color", glm::value_ptr(render.color));

		ImGui::SeparatorText("Mesh");
		const char* meshPreview = AssetRegistry<Mesh>::GetInstance().GetName(render.mesh) + 11;

		if (ImGui::BeginCombo("##mesh", meshPreview))
		{
			for (const auto& info : Mesh::PrimitiveMetas)
			{
				if(ImGui::Selectable(
					info.name,
					render.mesh == GetAssetID(Mesh::GetPrimitiveName(info.primitive))))
					render.mesh = GetAssetID(Mesh::GetPrimitiveName(info.primitive));
			}
			
			ImGui::EndCombo();
		}

		ImGui::SeparatorText("Shader");
		const char* shaderPreview = AssetRegistry<Shader>::GetInstance().GetName(render.shader);
		if (ImGui::BeginCombo("##shader", shaderPreview))
		{
			auto it = AssetRegistry<Shader>::GetInstance().GetNameMapBegin();
			auto endIt = AssetRegistry<Shader>::GetInstance().GetNameMapEnd();
			for (it; it != endIt; it++)
			{
				if (ImGui::Selectable(it->second.c_str(), render.shader == it->first))
					render.shader = it->first;
			}
			ImGui::EndCombo();
		}

		ImGui::SeparatorText("Texture");
		const char* texPreview = AssetRegistry<Texture>::GetInstance().GetName(render.texture);
		if (ImGui::BeginCombo("##texture", texPreview))
		{
			auto it = AssetRegistry<Texture>::GetInstance().GetNameMapBegin();
			auto endIt = AssetRegistry<Texture>::GetInstance().GetNameMapEnd();
			for (it; it != endIt; it++)
			{
				if (ImGui::Selectable(it->second.c_str(), render.texture == it->first))
					render.texture = it->first;
			}
			ImGui::EndCombo();
		}

		ImGui::NewLine();
	}

	void DrawUIInspector(EditorContext& eCtx, entt::registry& reg, UIComponent& ui)
	{
		if (ui.canvas != 0)
		{
			ImGui::SeparatorText("Canvas");
			auto canvasView = reg.view<CanvasComponent, NametagComponent>();
			auto curCanvas = reg.try_get<NametagComponent>(GetEntityByUUID(reg, ui.canvas));
			const char* canvasPreview = curCanvas != nullptr ? curCanvas->name.c_str() : "null";
			if (ImGui::BeginCombo("##canvas", canvasPreview))
			{
				//@TODO : Set Canvas, Hierarchy
				for (auto [e, canvas, tag] : canvasView.each())
				{
					if (ImGui::Selectable(tag.name.c_str(), ui.canvas == tag.id))
					{
						ui.canvas = tag.id;
						SetHierarchy(reg, GetEntityByUUID(reg, ui.canvas), eCtx.selectedEntity);
					}
				}
				ImGui::EndCombo();
			}
		}

		ImGui::SeparatorText("Sort Order");
		ImGui::InputInt("##ui Order", &ui.sortOrder, 0, 1000);

		ImGui::SeparatorText("UI Type");
		const char* typePreview = UITypeMetas[(int)ui.type].name;
		if (ImGui::BeginCombo("##uitypes", typePreview))
		{
			for (auto info : UITypeMetas)
			{
				if (ImGui::Selectable(info.name, ui.type == info.type, ImGuiSelectableFlags_Disabled))
					ui.type = info.type;
			}
			ImGui::EndCombo();
		}

		ImGui::NewLine();
	}

	void DrawCanvasInspector(EditorContext& eCtx, entt::registry& reg, CanvasComponent& canvas)
	{
		ImGui::SeparatorText("Render Mode");
		if (ImGui::BeginCombo("##renderMode", "ScreenOverlay"))
			ImGui::EndCombo();
		
		ImGui::SeparatorText("Size");
		ImGui::Text("Reference Size");// ImGui::SameLine();
		ImGui::DragFloat2("##refSize", glm::value_ptr(canvas.referenceSize));

		ImGui::Text("Actual Size");// ImGui::SameLine();
		ImGui::DragFloat2("##actSize", glm::value_ptr(canvas.actualSize));

		ImGui::SeparatorText("Sort Order");
		ImGui::InputInt("##canvasOrder", &canvas.sortOrder, 0, 1000);

		ImGui::NewLine();
	}

	void DrawRectTransformInspector(EditorContext& eCtx, entt::registry& reg, RectTransformComponent& rect)
	{
		auto ui = reg.try_get<UIComponent>(eCtx.selectedEntity);

		if (rect.anchorMin == rect.anchorMax)
		{
			ImGui::SeparatorText("Position");
			ImGui::Text("X"); ImGui::SameLine(); ImGui::SetNextItemWidth(60.f);
			ImGui::DragFloat("##anchoredPosx", &rect.anchoredPosition.x, 1.f, 0.f, 0.f, "%g"); ImGui::SameLine();
			ImGui::Text("Y"); ImGui::SameLine(); ImGui::SetNextItemWidth(60.f);
			ImGui::DragFloat("##anchoredPosy", &rect.anchoredPosition.y, 1.f, 0.f, 0.f, "%g");

			if (ui && ui->type != UIType::Text)
			{
				ImGui::SeparatorText("Size");
				ImGui::Text("Width"); ImGui::SameLine(); ImGui::SetCursorPosX(95.f);
				ImGui::Text("Height"); ImGui::SetNextItemWidth(80.f);
				ImGui::DragFloat("##sizewidth", &rect.sizeDelta.x, 1.f, 0.f, 0.f, "%g");
				ImGui::SameLine(); ImGui::SetNextItemWidth(80.f);
				ImGui::DragFloat("##sizeheight", &rect.sizeDelta.y, 1.f, 0.f, 0.f, "%g");
			}

			ImGui::SeparatorText("Offset");
			ImGui::Text("Min"); ImGui::SameLine(); ImGui::SetCursorPosX(50.f); ImGui::SameLine();
			ImGui::Text("X"); ImGui::SameLine(); ImGui::SetNextItemWidth(50.f);
			ImGui::DragFloat("##offsetMinX", &rect.offsetMin.x, 0.1f, 0.f, 1.f, "%.1f"); ImGui::SameLine();
			ImGui::Text("Y"); ImGui::SameLine(); ImGui::SetNextItemWidth(50.f);
			ImGui::DragFloat("##offsetMinY", &rect.offsetMin.y, 0.1f, 0.f, 1.f, "%.1f");

			ImGui::Text("Max");ImGui::SameLine(); ImGui::SetCursorPosX(40.f); ImGui::SameLine();
			ImGui::Text("X"); ImGui::SameLine(); ImGui::SetNextItemWidth(50.f); ImGui::SameLine();
			ImGui::DragFloat("##offsetMaxX", &rect.offsetMax.x, 0.1f, 0.f, 1.f, "%.1f"); ImGui::SameLine();
			ImGui::Text("Y"); ImGui::SameLine(); ImGui::SetNextItemWidth(50.f);
			ImGui::DragFloat("##offsetMaxY", &rect.offsetMax.x, 0.1f, 0.f, 1.f, "%.1f");

		}
		else
		{
			float offsetRight{ std::abs(rect.offsetMax.x) }, offsetTop{ std::abs(rect.offsetMax.y) };

			ImGui::SeparatorText("Margin"); // Offset? Margin?
			ImGui::Text("Left");
			ImGui::SameLine(); ImGui::SetCursorPosX(95.f);
			ImGui::Text("Top");

			ImGui::SetNextItemWidth(80.f);
			ImGui::InputFloat("##Left", &rect.offsetMin.x, 0.0f, 0.0f, "%g"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.f);
			if (ImGui::InputFloat("##Top", &offsetTop, 0.0f, 0.0f, "%g"))
				rect.offsetMax.y = -offsetTop;

			ImGui::Text("Right");
			ImGui::SameLine(); ImGui::SetCursorPosX(95.f);
			ImGui::Text("Bottom");
			ImGui::SetNextItemWidth(80.f);

			if(ImGui::InputFloat("##Right", &offsetRight, 0.0f, 0.0f, "%g"))
				rect.offsetMax.x = -offsetRight;
			ImGui::SameLine(); ImGui::SetNextItemWidth(80.f);
			ImGui::InputFloat("##Bottom", &rect.offsetMin.y, 0.0f, 0.0f, "%g");
		}

		ImGui::SeparatorText("Anchor");
		ImGui::Text("Min"); ImGui::SameLine(); ImGui::SetCursorPosX(50.f); ImGui::SameLine();
		ImGui::Text("X"); ImGui::SameLine(); ImGui::SetNextItemWidth(50.f);
		ImGui::DragFloat("##anchorMinX", &rect.anchorMin.x, 0.1f, 0.f, 1.f, "%.1f"); ImGui::SameLine();
		ImGui::Text("Y"); ImGui::SameLine(); ImGui::SetNextItemWidth(50.f);
		ImGui::DragFloat("##anchorMinY", &rect.anchorMin.y, 0.1f, 0.f, 1.f, "%.1f");

		ImGui::Text("Max");ImGui::SameLine(); ImGui::SetCursorPosX(40.f); ImGui::SameLine();
		ImGui::Text("X"); ImGui::SameLine(); ImGui::SetNextItemWidth(50.f);
		ImGui::DragFloat("##anchorMaxX", &rect.anchorMax.x, 0.1f, 0.f, 1.f, "%.1f"); ImGui::SameLine();
		ImGui::Text("Y"); ImGui::SameLine(); ImGui::SetNextItemWidth(50.f);
		ImGui::DragFloat("##anchorMaxY", &rect.anchorMax.y, 0.1f, 0.f, 1.f, "%.1f");

		ImGui::SeparatorText("Pivot");
		ImGui::Text("X"); ImGui::SameLine(); ImGui::SetNextItemWidth(60.f);
		ImGui::DragFloat("##pivotx", &rect.pivot.x, 0.1f, 0.f, 1.f, "%.1f"); ImGui::SameLine();
		ImGui::Text("Y"); ImGui::SameLine(); ImGui::SetNextItemWidth(60.f);
		ImGui::DragFloat("##pivoty", &rect.pivot.y, 0.1f, 0.f, 1.f, "%.1f");

		ImGui::NewLine();
	}

	void DrawTextInspector(EditorContext& eCtx, entt::registry& reg, TextComponent& text)
	{
		ImGui::SeparatorText("Text");
		if (ImGui::InputText("##text", &text.text))
			text.dirty = true;

		ImGui::SeparatorText("Color");
		ImGui::ColorEdit4("##textcolor", glm::value_ptr(text.color));

		ImGui::SeparatorText("Size");
		ImGui::DragFloat("##fontSize", &text.fontSize, 1.f, 0.0f, 0.0f, "%g");

		auto beginIt = AssetRegistry<Font>::GetInstance().GetNameMapBegin();
		auto endIt = AssetRegistry<Font>::GetInstance().GetNameMapEnd();
		const char* fontPreview = AssetRegistry<Font>::GetInstance().GetName(text.font);
		if (ImGui::BeginCombo("##fontCombo", fontPreview))
		{
			for (beginIt; beginIt != endIt; ++beginIt)
			{
				if (ImGui::Selectable(beginIt->second.c_str(), text.font == beginIt->first))
				{
					text.font = beginIt->first;
					text.dirty = true;
				}
			}
			ImGui::EndCombo();
		}

		ImGui::NewLine();
	}

	void DrawTargetInspector(EditorContext& eCtx, entt::registry& reg, TargetComponent& target)
	{
		ImGui::SeparatorText("Target");
		
		entt::entity targetEntity = GetEntityByUUID(reg, target.target);
		const char* namePreview = reg.try_get<NametagComponent>(targetEntity)->name.c_str();
		if (ImGui::BeginCombo("##entityCombo", namePreview))
		{
			auto view = reg.view<NametagComponent, TransformComponent>();
			for (auto [e, tag, transform] : view.each())
			{
				if (ImGui::Selectable(tag.name.c_str(), target.target == tag.id))
					target.target = tag.id;
			}
			ImGui::EndCombo();
		}

		DrawVec3Control("Offset", glm::value_ptr(target.headOffset));

		ImGui::NewLine();
	}

	void DrawSelectableInspector(EditorContext& eCtx, entt::registry& reg, SelectableComponent& selectable)
	{
		ImGui::SeparatorText("Interactable");
		ImGui::Text("Interactable"); ImGui::SameLine();
		ImGui::Checkbox("##interactable", &selectable.interactable);

		ImGui::SeparatorText("Normal Color");
		//ImGui::Text("Normal Color"); ImGui::SameLine();
		ImGui::ColorEdit4("##normalcolor", glm::value_ptr(selectable.normalColor));

		ImGui::SeparatorText("Highlighted Color");
		//ImGui::Text("Highlighted Color"); ImGui::SameLine();
		ImGui::ColorEdit4("##highlightedColor", glm::value_ptr(selectable.highlightedColor));

		ImGui::SeparatorText("Pressed Color");
		//ImGui::Text("Pressed Color"); ImGui::SameLine();
		ImGui::ColorEdit4("##pressedColor", glm::value_ptr(selectable.pressedColor));

		ImGui::NewLine();
	}

	bool DrawVec3Control(const char* label, float* vec, int flags)
	{
		ImGui::SeparatorText(label);

		float avail = ImGui::GetContentRegionAvail().x;
		float firstPos = avail / 3.f / 2.f;
		float textSize = ImGui::CalcTextSize("X").x;

		ImGui::SetCursorPosX(firstPos + textSize / 2.f);
		ImGui::Text("X"); ImGui::SameLine();

		textSize = ImGui::CalcTextSize("Y").x;
		ImGui::SetCursorPosX(firstPos += avail / 3.f + textSize / 2.f);
		ImGui::Text("Y"); ImGui::SameLine();

		textSize = ImGui::CalcTextSize("Z").x;
		ImGui::SetCursorPosX(firstPos += avail / 3.f + textSize / 2.f);
		ImGui::Text("Z");

		ImGui::PushItemWidth(avail);
		
		std::string tmpLabel = "##";
		tmpLabel += label;

		if (ImGui::DragFloat3(tmpLabel.c_str(), vec, 1.0f, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max(), "%.2f", flags))
			return true;

		ImGui::PopItemWidth();

		return false;
	}
}

