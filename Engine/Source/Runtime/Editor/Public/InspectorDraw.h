#ifndef MANGO_INSPECTORDRAW_H
#define MANGO_INSPECTORDRAW_H

#include <glm/vec3.hpp>
#include <entt/fwd.hpp>
#include "ECS/Components/ComponentFwd.h"

namespace tomato
{
	struct EditorContext;

	void DrawCameraInspcetor(EditorContext&, entt::registry&, CameraComponent&);
	void DrawTransformInspector(EditorContext&, entt::registry&, TransformComponent&);
	void DrawMovementInspector(EditorContext&, entt::registry&, MovementComponent&);
	void DrawVelocityInspector(EditorContext&, entt::registry&, VelocityComponent&);
	void DrawColliderInspector(EditorContext&, entt::registry&, ColliderComponent&);
	void DrawRenderInspector(EditorContext&, entt::registry&, RenderComponent&);
	void DrawUIInspector(EditorContext&, entt::registry&, UIComponent&);
	void DrawCanvasInspector(EditorContext&, entt::registry&, CanvasComponent&);
	void DrawRectTransformInspector(EditorContext&, entt::registry&, RectTransformComponent&);

	
	
	bool DrawVec3Control(const char* label, float* vec, int flags = 0);
}


#endif // MANGO_INSPECTORDRAW_H
