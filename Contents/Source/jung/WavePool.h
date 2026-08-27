#ifndef MANGO_WAVEPOOL_H
#define MANGO_WAVEPOOL_H

#include <entt/fwd.hpp>
#include <glm/vec3.hpp>

#include "Containers/EntityPool.h"

struct WavePoolTraits
{
	static void Assemble(entt::registry& registry_, entt::entity wave);

	static void Reset(entt::registry& registry_, entt::entity e,
		entt::entity owner, glm::vec3 pos, float speed);

	static bool Deactivate(entt::registry& registry_, entt::entity e);

};
using WavePool = tomato::EntityPool<WavePoolTraits>;

#endif // !MANGO_WAVEPOOL_H
