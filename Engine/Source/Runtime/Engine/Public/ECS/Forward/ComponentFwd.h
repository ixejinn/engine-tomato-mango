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
	struct PhysicsBodyComponent;
	struct FrictionComponent;
	struct SurfaceComponent;

	struct RenderComponent;
	struct StencilComponent;

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

	struct ActiveTag;
	struct RootEntityTag;
	struct MainCameraTag;
	struct CharacterTag;
	struct GroundTriggerTag;
	struct RollbackEntityTag;

	struct EditorHidden;
	struct NoInspector;
}

#endif // !MANGO_COMPONENTFWD_H
