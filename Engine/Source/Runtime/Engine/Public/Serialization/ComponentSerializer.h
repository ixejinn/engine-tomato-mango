#ifndef MANGO_COMPONENTSERIALIZER_H
#define MANGO_COMPONENTSERIALIZER_H

#include <entt/fwd.hpp>
#include "Serialization/Json.h"
#include "ECS/Forward/ComponentFwd.h"

namespace tomato::Serialization
{
	// ActiveTag.h
	inline void Save(json&, entt::registry&, const ActiveTag&) {}
	inline void Load(const json&, entt::registry&, ActiveTag&) {}

	// Camera.h
	void Save(json&, entt::registry&, const CameraComponent&);
	void Load(const json&, entt::registry&, CameraComponent&);

	inline void Save(json&, entt::registry&, const MainCameraTag&) {}
	inline void Load(const json&, entt::registry&, MainCameraTag&) {}

	// Character.h
	inline void Save(json&, entt::registry&, const CharacterTag&) {}
	inline void Load(const json&, entt::registry&, CharacterTag&) {}

	inline void Save(json&, entt::registry&, const GroundTriggerTag&) {}
	inline void Load(const json&, entt::registry&, GroundTriggerTag&) {}

	// Collision.h
	void Save(json&, entt::registry&, const ColliderComponent&);
	void Load(const json&, entt::registry&, ColliderComponent&);

	// EditorTag.h
	inline void Save(json&, entt::registry&, const EditorHiddenTag&) {}
	inline void Load(const json&, entt::registry&, EditorHiddenTag&) {}

	inline void Save(json&, entt::registry&, const NoInspectorTag&) {}
	inline void Load(const json&, entt::registry&, NoInspectorTag&) {}

	// Hierarchy.h
	//  엔티티 ID 대신 UUID를 저장
	inline void Save(json&, entt::registry&, const RootEntityTag&) {}
	inline void Load(const json&, entt::registry&, RootEntityTag&) {}

	void Save(json&, entt::registry&, const HierarchyComponent&);
	void Load(const json&, entt::registry&, HierarchyComponent&);

	// Movement.h
	void Save(json&, entt::registry&, const InputChannelComponent&);
	void Load(const json&, entt::registry&, InputChannelComponent&);

	void Save(json&, entt::registry&, const MovementComponent&);
	void Load(const json&, entt::registry&, MovementComponent&);

	// Nametag.h

	// Particle.h
	void Save(json&, entt::registry&, const ParticleEmitterComponent&);
	void Load(const json&, entt::registry&, ParticleEmitterComponent&);

	void Save(json&, entt::registry&, const ParticleSimulationComponent&);
	void Load(const json&, entt::registry&, ParticleSimulationComponent&);

	// PhysicsBody.h
	void Save(json&, entt::registry&, const PhysicsBodyComponent&);
	void Load(const json&, entt::registry&, PhysicsBodyComponent&);

	// Render.h
	void Save(json&, entt::registry&, const RenderComponent&);
	void Load(const json&, entt::registry&, RenderComponent&);

	void Save(json&, entt::registry&, const StencilComponent&);
	void Load(const json&, entt::registry&, StencilComponent&);

	// Rollback.h
	inline void Save(json&, entt::registry&, const RollbackEntityTag&) {}
	inline void Load(const json&, entt::registry&, RollbackEntityTag&) {}

	// Target.h
	void Save(json&, entt::registry&, const TargetComponent&);
	void Load(const json&, entt::registry&, TargetComponent&);

	// Text.h
	void Save(json&, entt::registry&, const TextComponent&);
	void Load(const json&, entt::registry&, TextComponent&);

	// Transform.h
	void Save(json&, entt::registry&, const TransformComponent&);
	void Load(const json&, entt::registry&, TransformComponent&);

	// UI.h
	void Save(json&, entt::registry&, const CanvasComponent&);
	void Load(const json&, entt::registry&, CanvasComponent&);

	void Save(json&, entt::registry&, const UIComponent&);
	void Load(const json&, entt::registry&, UIComponent&);

	void Save(json&, entt::registry&, const RectTransformComponent&);
	void Load(const json&, entt::registry&, RectTransformComponent&);

	void Save(json&, entt::registry&, const SelectableComponent&);
	void Load(const json&, entt::registry&, SelectableComponent&);

	// UIEvents.h	////////////////////////////////////////////////////
	inline void Save(json&, entt::registry&, const MouseEventComponent&) {}
	inline void Load(const json&, entt::registry&, MouseEventComponent&) {}

	// Velocity.h
	void Save(json&, entt::registry&, const VelocityComponent&);
	void Load(const json&, entt::registry&, VelocityComponent&);

	void Save(json&, entt::registry&, const FrictionComponent&);
	void Load(const json&, entt::registry&, FrictionComponent&);

	void Save(json&, entt::registry&, const SurfaceComponent&);
	void Load(const json&, entt::registry&, SurfaceComponent&);

	// Visibility.h
	void Save(json&, entt::registry&, const VisibilityComponent&);
	void Load(const json&, entt::registry&, VisibilityComponent&);
}

#endif // !MANGO_COMPONENTSERIALIZER_H
