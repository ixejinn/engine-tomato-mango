#ifndef MANGO_WAVECOMPONENT_H
#define MANGO_WAVECOMPONENT_H

#include <entt/entt.hpp>
#include <glm/vec3.hpp>
#include <stdint.h>
#include <vector>

namespace tomato
{
	struct WaveComponent
	{
		bool active{ false };

		glm::vec3 origin{ 0 };
		float radius;
		float speed;

		glm::vec3 direction{ 0 };
		int64_t startTick{ 0 };

		entt::entity owner{ entt::null };
		std::vector<entt::entity> colliders;
	};

	//struct WaveColliderComponent
	//{
	//	entt::entity wave{ entt::null };
	//};

	struct WaveColliderTag {};
}

#endif // MANGO_WAVECOMPONENT_H
