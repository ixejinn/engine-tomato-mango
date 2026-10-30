#ifndef MANGO_TARGETCOMPONENT_H
#define MANGO_TARGETCOMPONENT_H

#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include "Serialization/UUID.h"

namespace tomato
{
	struct TargetComponent
	{
		entt::entity target{ entt::null };
		glm::vec3 headOffset{ 0.f, 0.f, 0.f };
	};
}
#endif // !MANGO_TARGETCOMPONENT_H
