#include "WaveManager.h"
#include <entt/entt.hpp>

#include <ECS/Components/Character.h>
#include "WaveComponent.h"
using namespace tomato;

entt::entity WaveManager::Acquire(entt::registry& registry, entt::entity owner, glm::vec3 pos, float speed, float radius)
{
	entt::entity wave = wavePool_.Acquire(registry, owner, pos, speed, radius);
	if (wave == entt::null)
		return wave;

	auto& waveComp = registry.get<WaveComponent>(wave);
	//waveComp.colliders.reserve(MAX_PLAYER_NUM);

	auto playerView = registry.view<CharacterTag>();
	for (auto player : playerView)
	{
		if (player == waveComp.owner) continue;

		entt::entity collider = colliderPool_.Acquire(registry, wave, player);
		waveComp.colliders.push_back(collider);
	}

	return wave;
}

void WaveManager::Release(entt::registry& registry, entt::entity wave)
{
	auto& waveComp = registry.get<WaveComponent>(wave);
	for (auto collider : waveComp.colliders)
		colliderPool_.Release(registry, collider);
	waveComp.colliders.clear();

	wavePool_.Release(registry, wave);
}