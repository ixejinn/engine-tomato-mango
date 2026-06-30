#include "InspectorDraw.h"

#include <entt/entt.hpp>

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "ECS/Components/Transform.h"
namespace tomato
{
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

	bool DrawVec3Control(const char* label, float* vec)
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

		if (ImGui::DragFloat3(tmpLabel.c_str(), vec, 0.1f, 0.f, 100.f, "%.2f"))
			return true;

		ImGui::PopItemWidth();

		return false;
	}
}

