#ifndef MANGO_TARGETCOMPONENT_H
#define MANGO_TARGETCOMPONENT_H

#include <glm/glm.hpp>
#include <entt/entt.hpp>

namespace tomato
{
	struct TargetComponent
	{
		entt::entity target{entt::null};
		glm::vec3 headOffset{0};
	};
}
#endif // !MANGO_TARGETCOMPONENT_H
