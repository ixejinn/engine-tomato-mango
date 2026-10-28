#include "Serialization/ComponentRegistry.h"

#include "ECS/Components/ActiveTag.h"
#include "ECS/Components/Visibility.h"
#include "ECS/Components/Camera.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Velocity.h"
#include "ECS/Components/Collision.h"
#include "ECS/Components/PhysicsBody.h"
#include "ECS/Components/Render.h"
#include "ECS/Components/UI.h"
#include "ECS/Components/UIEvents.h"
#include "ECS/Components/Text.h"
#include "ECS/Components/Target.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/Character.h"
#include "ECS/Components/Particle.h"
#include "ECS/Components/EditorTag.h"

#include "Utils/Bitmask/BitmaskOperators.h"
namespace tomato::Serialization
{
	void ComponentRegistry::Init()
	{
		if (initialized)
			return;

		RegisterBuiltInComponents();
		RegisterBuiltInComponentsInspector();

		initialized = true;
	}

	// Register built-in engine components explicitly during Engine initialization.
	// Register custom components statically using a macro in each component's .cpp file.
	void ComponentRegistry::RegisterBuiltInComponents()
	{
		RegisterComponent<VisibilityComponent>("Visibility", ComponentCategory::Common,
			ComponentFlags::Hidden | ComponentFlags::ReadOnly | ComponentFlags::Essential);
		RegisterComponent<CameraComponent>("Camera", ComponentCategory::Common);
		RegisterComponent<InputChannelComponent>("InputChannel", ComponentCategory::Common);
		RegisterComponent<TransformComponent>("Transform", ComponentCategory::Common , ComponentFlags::Essential);
		RegisterComponent<MovementComponent>("Movement", ComponentCategory::Common);
		RegisterComponent<VelocityComponent>("Velocity", ComponentCategory::Common);
		RegisterComponent<ColliderComponent>("Collider", ComponentCategory::Common);
		RegisterComponent<PhysicsBodyComponent>("PhysicsBody", ComponentCategory::Common);
		RegisterComponent<RenderComponent>("Render", ComponentCategory::Common);
		RegisterComponent<UIComponent>("UI", ComponentCategory::UI);
		RegisterComponent<CanvasComponent>("Canvas", ComponentCategory::UI);
		RegisterComponent<RectTransformComponent>("Rect Transform", ComponentCategory::UI, ComponentFlags::Essential);
		RegisterComponent<TextComponent>("Text", ComponentCategory::UI);
		RegisterComponent<TargetComponent>("Target", ComponentCategory::Common);
		RegisterComponent<SelectableComponent>("Selectable", ComponentCategory::UI);
		RegisterComponent<MouseEventComponent>("Mouse Event", ComponentCategory::UI);
		RegisterComponent<HierarchyComponent>("Hierarchy", ComponentCategory::Common, ComponentFlags::Hidden);
		RegisterComponent<ParticleEmitterComponent>("Particle Effect", ComponentCategory::Particle);
		RegisterComponent<ParticleRenderComponent>("Particle Render", ComponentCategory::Particle, ComponentFlags::Hidden);
		
		//Tag
		RegisterComponent<ActiveTag>("TagActive", ComponentCategory::Tag, ComponentFlags::Hidden);
		RegisterComponent<RootEntityTag>("TagRoot", ComponentCategory::Tag, ComponentFlags::Hidden);
		RegisterComponent<MainCameraTag>("TagMainCam", ComponentCategory::Tag);
		RegisterComponent<CharacterTag>("TagCharacter", ComponentCategory::Tag, ComponentFlags::Hidden);
		RegisterComponent<EditorHidden>("TagEditorHidden", ComponentCategory::Tag, ComponentFlags::Hidden);
		RegisterComponent<NoInspector>("TagNoInspector", ComponentCategory::Tag, ComponentFlags::Hidden);
	}

	void ComponentRegistry::RegisterBuiltInComponentsInspector()
	{
		//@Warning : Must initialize ComponentRegistry::RegisterBuiltInComponents() first.

		RegisterInspector<CameraComponent>("Camera", DrawCameraInspcetor);
		RegisterInspector<InputChannelComponent>("InputChannel", DrawInputChannelInspcetor);
		RegisterInspector<TransformComponent>("Transform", DrawTransformInspector);
		RegisterInspector<MovementComponent>("Movement", DrawMovementInspector);
		RegisterInspector<VelocityComponent>("Velocity", DrawVelocityInspector);
		RegisterInspector<ColliderComponent>("Collider", DrawColliderInspector);
		RegisterInspector<RenderComponent>("Render", DrawRenderInspector);
		RegisterInspector<UIComponent>("UI", DrawUIInspector);
		RegisterInspector<CanvasComponent>("Canvas", DrawCanvasInspector);
		RegisterInspector<RectTransformComponent>("Rect Transform", DrawRectTransformInspector);
		RegisterInspector<TextComponent>("Text", DrawTextInspector);
		RegisterInspector<TargetComponent>("Target", DrawTargetInspector);
		RegisterInspector<SelectableComponent>("Selectable", DrawSelectableInspector);
		RegisterInspector<ParticleEmitterComponent>("Particle Effect", DrawParticleInspector);
	}

	const ComponentInfo* ComponentRegistry::FindComponentInfo(const std::string& name) const
	{
		auto it = nameToIndex_.find(name);
	
		if(it == nameToIndex_.end())
			return nullptr;

		return &componentInfo_[it->second];
	}

	bool ComponentRegistry::HasComponentName(const std::string& name) const
	{
		return nameToIndex_.contains(name);
	}
}