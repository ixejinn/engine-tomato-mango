#include "Serialization/ComponentRegistry.h"

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

		RegisterComponent<CameraComponent>("Camera");
		RegisterComponent<InputChannelComponent>("InputChannel");
		RegisterComponent<TransformComponent>("Transform");
		RegisterComponent<MovementComponent>("Movement");
		RegisterComponent<VelocityComponent>("Velocity");
		RegisterComponent<ColliderComponent>("Collider");
		RegisterComponent<RenderComponent>("Render");
		RegisterComponent<UIComponent>("UI");
		RegisterComponent<CanvasComponent>("Canvas");
		RegisterComponent<RectTransformComponent>("RectTransform");
		RegisterComponent<TextComponent>("Text");
		RegisterComponent<TargetComponent>("Target");
		RegisterComponent<SelectableComponent>("Selectable");
		RegisterComponent<MouseEventComponent>("MouseEvent");
		RegisterComponent<HierarchyComponent>("Hierarchy");
		RegisterComponent<RootEntityTag>("TagRoot");
		RegisterComponent<MainCameraTag>("TagMainCam");


		initialized = true;
	}

	void ComponentRegistry::InitInspector()
	{
		//@Warning : Must initialize ComponentRegistry::Init() first.
		RegisterInspector<TransformComponent>("Transform", DrawTransformInspector);
	}

	const ComponentInfo* ComponentRegistry::FindComponentInfo(const std::string& name) const
	{
		auto it = nameToIndex_.find(name);
	
		if(it == nameToIndex_.end())
			return nullptr;

		return &componentInfo_[it->second];
	}
}