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
	WaveManager(
		const tomato::PassKey<tomato::State>& key, entt::registry& reg, std::size_t poolSize)
		:	registry_(reg), poolSize_(poolSize),
			wavePool_(key, reg, poolSize), colliderPool_(key, reg, poolSize * tomato::MAX_PLAYER_NUM)
	{}

	/*WaveInstance Acquire(entt::entity owner, glm::vec3 pos, float speed, float radius);
	void Release(WaveInstance* wave);*/

	entt::entity Acquire(entt::entity owner, glm::vec3 pos, float speed, float radius = 10.f);
	void Release(entt::entity wave);
private:
	entt::registry& registry_;
	std::size_t poolSize_;

	tomato::EntityPool<WavePoolTraits> wavePool_;
	tomato::EntityPool<WaveColliderPoolTraits> colliderPool_;

};

#endif // !MANGO_WAVEMANAGER_H
