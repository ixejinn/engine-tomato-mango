#include "WaveManager.h"
#include <entt/entt.hpp>

#include <ECS/Components/Character.h>
#include "WaveComponent.h"
using namespace tomato;

entt::entity WaveManager::Acquire(entt::entity owner, glm::vec3 pos, float speed, float radius)
{
	entt::entity wave = wavePool_.Acquire(owner, pos, speed, radius);
	if (wave == entt::null)
		return wave;


	auto& waveComp = registry_.get<WaveComponent>(wave);
	//waveComp.colliders.reserve(MAX_PLAYER_NUM);

	auto playerView = registry_.view<CharacterTag>();
	for (auto player : playerView)
	{
		if (player == waveComp.owner) continue;

		entt::entity collider = colliderPool_.Acquire(wave, player);
		waveComp.colliders.push_back(collider);
	}

	return wave;
}

void WaveManager::Release(entt::entity wave)
{
	auto& waveComp = registry_.get<WaveComponent>(wave);
	for (auto collider : waveComp.colliders)
		colliderPool_.Release(collider);
	waveComp.colliders.clear();

	wavePool_.Release(wave);
}