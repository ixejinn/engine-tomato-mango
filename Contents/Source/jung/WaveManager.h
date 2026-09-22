#ifndef MANGO_WAVEMANAGER_H
#define MANGO_WAVEMANAGER_H

#include <vector>
#include <entt/fwd.hpp>
#include <GameplayConfig.h>

#include "Utils/PassKey.h"
#include "State/StateFwd.h"
#include "WavePool.h"
#include "WaveColliderPool.h"

struct WaveInstance
{
	entt::entity wave_;
	std::vector<entt::entity> colliders_;
};

class WaveManager
{
public:
	explicit WaveManager(
		tomato::EntityPool<WavePoolTraits>& wavePool, tomato::EntityPool<WaveColliderPoolTraits>& colliderPool)
			: wavePool_(wavePool), colliderPool_(colliderPool) {}

	/*WaveInstance Acquire(entt::entity owner, glm::vec3 pos, float speed, float radius);
	void Release(WaveInstance* wave);*/

	entt::entity Acquire(entt::registry& registry, entt::entity owner, glm::vec3 pos, float speed, float radius = 10.f);
	void Release(entt::registry& registry, entt::entity wave);

private:
	tomato::EntityPool<WavePoolTraits>& wavePool_;
	tomato::EntityPool<WaveColliderPoolTraits>& colliderPool_;
};

#endif // !MANGO_WAVEMANAGER_H
