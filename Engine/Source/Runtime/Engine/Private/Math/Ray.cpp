#include "Math/Ray.h"

#include <glm/mat4x4.hpp>
#include <entt/entt.hpp>

#include "Services/Window.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "ECS/Components/Camera.h"
#include "ECS/Components/Transform.h"

namespace tomato
{
	Ray ScreenPointToRay(entt::registry& reg, glm::vec2 mousePos)
	{
		// 1. Screen to NDC
		float x = (2.f * mousePos.x) / Window::GetWidth() - 1.0f;
		float y = 1.f - (2.f * mousePos.y) / Window::GetHeight();

		// 2. Clip(NDC) to View (inverse Projection)
		glm::vec4 rayClip(x, y, -1.f, 1.f);

		auto& renderCtx = reg.ctx().get<RenderContext>();;
		auto& camera = reg.get<CameraComponent>(renderCtx.mainCam);

		glm::vec4 rayEye = glm::inverse(camera.projection) * rayClip;

		rayEye.z = -1.f;
		rayEye.w = 0.f;

		// 3. View To World
		glm::vec3 rayDir = glm::normalize(glm::vec3(glm::inverse(camera.view) * rayEye));
		glm::vec3 rayOrigin = reg.get<TransformComponent>(renderCtx.mainCam).GetWorldPosition();

		return Ray{ rayOrigin, rayDir };
	}
}