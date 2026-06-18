#include "Serialization/ComponentRegistry.h"

#include "ECS/Components/Camera.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Rigidbody.h"
#include "ECS/Components/Render.h"
#include "ECS/Components/UI.h"
#include "ECS/Components/Text.h"

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
		RegisterComponent<RenderComponent>("Render");
		RegisterComponent<UIComponent>("UI");
		RegisterComponent<CanvasComponent>("Canvas");
		RegisterComponent<RectTransformComponent>("RectTransform");
		RegisterComponent<TargetComponent>("Target");
		RegisterComponent<SelectableComponent>("Selectable");
		RegisterComponent<TextComponent>("Text");

		initialized = true;
	}

	const ComponentInfo* ComponentRegistry::FindComponentInfo(const std::string& name) const
	{
		auto it = nameToIndex_.find(name);
	
		if(it == nameToIndex_.end())
			return nullptr;

		return &componentInfo_[it->second];
	}
}