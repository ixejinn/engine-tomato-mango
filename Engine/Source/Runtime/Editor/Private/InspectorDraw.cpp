#include "InspectorDraw.h"

#include <limits>
#include <entt/entt.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Resource/AssetRegistry.h"
#include "Resource/Render/Mesh.h"

#include "GLFW/glfw3.h"
#include "imgui.h"
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
#include "ECS/Components/Hierarchy.h"

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
	}

	void DrawMovementInspector(EditorContext& eCtx, entt::registry& reg, MovementComponent& movement)
	{
		ImGui::SeparatorText("Speed");
		ImGui::DragFloat("Speed", &movement.horizontalSpeed, 1.0f, 0.f, 100.f, "%.2f");
	}

	void DrawVelocityInspector(EditorContext& eCtx, entt::registry& reg, VelocityComponent& vel)
	{
		float velVec3[3] = { vel.velocity.x, vel.velocity.y, vel.velocity.z };

		ImGuiSliderFlags flags = ImGuiSliderFlags_NoInput;
		if (DrawVec3Control("Velocity", velVec3, flags))
			vel.velocity = glm::vec3(velVec3[0], velVec3[1], velVec3[2]);
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
		const char* typePreview = collider.type == ColliderType::Cube ? "Cube" : "Sphere";
		if (ImGui::BeginCombo("##Type", typePreview))
		{
			if (ImGui::Selectable("Cube", collider.type == ColliderType::Cube))
			{
				collider.type = ColliderType::Cube;
				collider.aabbDirty = true;
			}

			if (ImGui::Selectable("Sphere", collider.type == ColliderType::Sphere))
			{
				collider.type = ColliderType::Sphere;
				collider.aabbDirty = true;
			}

			ImGui::EndCombo();
		}

		ImGui::SeparatorText("Trigger");
		if (ImGui::RadioButton("T", collider.isTrigger == true))
			collider.isTrigger = true;

		ImGui::SameLine();

		if (ImGui::RadioButton("F", collider.isTrigger == false))
			collider.isTrigger = false;
	}

	void DrawRenderInspector(EditorContext& eCtx, entt::registry& reg, RenderComponent& render)
	{
		ImGui::SeparatorText("Color");
		ImGui::ColorEdit4("##color", glm::value_ptr(render.color));

		ImGui::SeparatorText("Mesh");
		const char* meshPreview = "Plain";

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

