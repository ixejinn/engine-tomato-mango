#ifndef MANGO_COMPONENTFWD_H
#define MANGO_COMPONENTFWD_H

namespace tomato
{
	// ActiveTag.h
	struct ActiveTag;

	// Camera.h
	struct CameraComponent;
	struct MainCameraTag;

	// Character.h
	struct CharacterTag;
	struct GroundTriggerTag;

	// Collision.h
	struct ColliderComponent;

	// EditorTag.h
	struct EditorHiddenTag;
	struct NoInspectorTag;
	struct NoSaveTag;

	// Hierarchy.h
	struct RootEntityTag;
	struct HierarchyComponent;

	// Movement.h
	struct InputChannelComponent;
	struct MovementComponent;

	// Nametag.h
	struct NametagComponent;

	// Particle.h	/////////////////////////////////
	struct ParticleEmitterComponent;
	struct ParticleRuntimeComponent;
	struct ParticleBufferComponent;
	struct ParticleRenderComponent;
	struct ParticleAttachmentComponent;

	// PhysicsBody.h
	struct PhysicsBodyComponent;

	// Render.h
	struct RenderComponent;
	struct StencilComponent;

	// Rollback.h
	struct RollbackEntityTag;

	// Target.h
	struct TargetComponent;

	// Text.h
	struct TextComponent;

	// Transform.h
	struct TransformComponent;

	// UI.h
	struct CanvasComponent;
	struct UIComponent;
	struct RectTransformComponent;
	struct SelectableComponent;

	// UIEvents.h
	struct MouseEventComponent;

	// Velocity.h
	struct VelocityComponent;
	struct FrictionComponent;
	struct SurfaceComponent;

	// Visibility.h
	struct VisibilityComponent;
}

#endif // !MANGO_COMPONENTFWD_H
