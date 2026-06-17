#include "Serialization/ComponentRegistry.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Render.h"
#include "ECS/Components/UI.h"

namespace tomato::Serialization
{
	void ComponentRegistry::Init()
	{
		if (initialized)
			return;

		RegisterComponent<TransformComponent>("Transform");
		RegisterComponent<RenderComponent>("Render");

		initialized = true;
	}
}