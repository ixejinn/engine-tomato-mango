#ifndef MANGO_INSPECTORDRAW_H
#define MANGO_INSPECTORDRAW_H

#include <glm/vec3.hpp>
#include <filesystem>
#include <entt/fwd.hpp>
#include "ECS/Forward/ComponentFwd.h"

namespace tomato
{
	struct EditorContext;

	bool DrawCameraInspcetor(EditorContext&, entt::registry&, CameraComponent&);
	bool DrawTransformInspector(EditorContext&, entt::registry&, TransformComponent&);
	bool DrawMovementInspector(EditorContext&, entt::registry&, MovementComponent&);
	bool DrawVelocityInspector(EditorContext&, entt::registry&, VelocityComponent&);
	bool DrawColliderInspector(EditorContext&, entt::registry&, ColliderComponent&);
	bool DrawRenderInspector(EditorContext&, entt::registry&, RenderComponent&);
	bool DrawUIInspector(EditorContext&, entt::registry&, UIComponent&);
	bool DrawCanvasInspector(EditorContext&, entt::registry&, CanvasComponent&);
	bool DrawRectTransformInspector(EditorContext&, entt::registry&, RectTransformComponent&);
	bool DrawTextInspector(EditorContext&, entt::registry&, TextComponent&);
	bool DrawTargetInspector(EditorContext&, entt::registry&, TargetComponent&);
	bool DrawSelectableInspector(EditorContext&, entt::registry&, SelectableComponent&);
	bool DrawParticleInspector(EditorContext&, entt::registry&, ParticleEmitterComponent&);

	
	std::filesystem::path DrawLoadResourceButton(const char* title, const char* filter, const std::filesystem::path& initDir);
	bool DrawVec3Control(const char* label, float* vec, int flags = 0);
}


#endif // MANGO_INSPECTORDRAW_H
