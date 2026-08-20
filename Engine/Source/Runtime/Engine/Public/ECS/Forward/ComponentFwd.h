#ifndef MANGO_COMPONENTFWD_H
#define MANGO_COMPONENTFWD_H

namespace tomato
{
	struct VisibilityComponent;

	struct CameraComponent;
	struct TransformComponent;
	struct InputChannelComponent;
	struct MovementComponent;
	struct VelocityComponent;
	struct ColliderComponent;
	struct RenderComponent;

	struct UIComponent;
	struct CanvasComponent;
	struct RectTransformComponent;
	struct TextComponent;
	struct TargetComponent;
	struct MouseEventComponent;
	struct SelectableComponent;

	struct HierarchyComponent;
	
	struct ParticleEmitterComponent;
	struct ParticleRenderComponent;

	struct RootEntityTag;
	struct MainCameraTag;
	struct CharacterTag;

	struct EditorHidden;
	struct NoInspector;
}

#endif // !MANGO_COMPONENTFWD_H
