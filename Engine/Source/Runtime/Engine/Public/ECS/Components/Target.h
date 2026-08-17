#ifndef MANGO_TARGETCOMPONENT_H
#define MANGO_TARGETCOMPONENT_H

#include <glm/glm.hpp>
#include "UUID.h"

namespace tomato
{
	struct TargetComponent
	{
		UUID target{ 0 };
		glm::vec3 headOffset{ 0.f, 0.f, 0.f };

		bool init{ false };
		glm::vec3 dir;
	};
}
#endif // !MANGO_TARGETCOMPONENT_H
