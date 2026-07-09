#include "Serialization/ComponentRegistry.h"

#include "ECS/Components/Visibility.h"
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

namespace tomato::Serialization
{
	void ComponentRegistry::Init()
	{
		if (initialized)
			return;

		//RegisterComponent<NametagComponent>("Nametag", ComponentCategory::Common,
		//	ComponentFlags::Hidden | ComponentFlags::ReadOnly | ComponentFlags::Essential);
		RegisterComponent<VisibilityComponent>("Visibility", ComponentCategory::Common,
			ComponentFlags::Hidden | ComponentFlags::ReadOnly | ComponentFlags::Essential);
		RegisterComponent<CameraComponent>("Camera", ComponentCategory::Common);
		RegisterComponent<InputChannelComponent>("InputChannel", ComponentCategory::Common);
		RegisterComponent<TransformComponent>("Transform", ComponentCategory::Common);
		RegisterComponent<MovementComponent>("Movement", ComponentCategory::Common);
		RegisterComponent<VelocityComponent>("Velocity", ComponentCategory::Common);
		RegisterComponent<ColliderComponent>("Collider", ComponentCategory::Common);
		RegisterComponent<RenderComponent>("Render", ComponentCategory::Common);
		RegisterComponent<UIComponent>("UI", ComponentCategory::UI);
		RegisterComponent<CanvasComponent>("Canvas", ComponentCategory::UI);
		RegisterComponent<RectTransformComponent>("RectTransform", ComponentCategory::UI);
		RegisterComponent<TextComponent>("Text", ComponentCategory::UI);
		RegisterComponent<TargetComponent>("Target", ComponentCategory::UI);
		RegisterComponent<SelectableComponent>("Selectable", ComponentCategory::UI);
		RegisterComponent<MouseEventComponent>("MouseEvent", ComponentCategory::UI);
		RegisterComponent<HierarchyComponent>("Hierarchy", ComponentCategory::Common);
		RegisterComponent<RootEntityTag>("TagRoot", ComponentCategory::Tag);
		RegisterComponent<MainCameraTag>("TagMainCam", ComponentCategory::Tag);


		initialized = true;
	}

	void ComponentRegistry::InitInspector()
	{
		//@Warning : Must initialize ComponentRegistry::Init() first.

		//RegisterInspector<NametagComponent>("Nametag", );
		RegisterInspector<CameraComponent>("Camera", DrawCameraInspcetor);
		//RegisterInspector<InputChannelComponent>("InputChannel", DrawInputChannelInspcetor);
		RegisterInspector<TransformComponent>("Transform", DrawTransformInspector);
		RegisterInspector<MovementComponent>("Movement", DrawMovementInspector);
		RegisterInspector<VelocityComponent>("Velocity", DrawVelocityInspector);
		RegisterInspector<ColliderComponent>("Collider", DrawColliderInspector);
		RegisterInspector<RenderComponent>("Render", DrawRenderInspector);
		RegisterInspector<UIComponent>("UI", DrawUIInspector);
		RegisterInspector<CanvasComponent>("Canvas", DrawCanvasInspector);
		RegisterInspector<RectTransformComponent>("RectTransform", DrawRectTransformInspector);
		RegisterInspector<TextComponent>("Text", DrawTextInspector);
		RegisterInspector<TargetComponent>("Target", DrawTargetInspector);
		RegisterInspector<SelectableComponent>("Selectable", DrawSelectableInspector);
	}

	const ComponentInfo* ComponentRegistry::FindComponentInfo(const std::string& name) const
	{
		auto it = nameToIndex_.find(name);
	
		if(it == nameToIndex_.end())
			return nullptr;

		return &componentInfo_[it->second];
	}
}