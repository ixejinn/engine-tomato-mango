#ifndef MANGO_PROJECTILECOMPONENT_H
#define MANGO_PROJECTILECOMPONENT_H

#include <glm/vec3.hpp>
namespace tomato
{
	// 현재 Transform 위치에서 end까지 이동
	struct MoveBetweenComponent
	{
		glm::vec3 start{ 0.f };
		glm::vec3 end{ 0.f };
	};
}

#endif // MANGO_PROJECTILECOMPONENT_H
