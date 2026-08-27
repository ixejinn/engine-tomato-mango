#ifndef MANGO_WAVECOMPONENT_H
#define MANGO_WAVECOMPONENT_H

#include <entt/entt.hpp>
#include <glm/vec3.hpp>
#include <stdint.h>
namespace tomato
{
	struct WaveComponent
	{
		bool active{ false };

		glm::vec3 origin{ 0 };
		float radius;
		float speed;

		int64_t startTick{ 0 };

		entt::entity owner{ entt::null };
	};

	struct WaveColliderComponent
	{
		entt::entity wave;
	};

	struct WaveColliderTag {};
}

#endif // MANGO_WAVECOMPONENT_H
