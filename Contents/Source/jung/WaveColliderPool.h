#ifndef MANGO_WAVECOLLIDERPOOL_H
#define MANGO_WAVECOLLIDERPOOL_H

#include "Containers/EntityPool.h"
#include "WaveComponent.h"
#include "WaveCollisionComponent.h"

struct WaveColliderPoolTraits
{
	static void Assemble(entt::registry& registry_, entt::entity e);
	static void Reset(entt::registry& registry_, entt::entity e,
		entt::entity wave, entt::entity target);
	static bool Deactivate(entt::registry& registry_, entt::entity e);
};

using WaveColliderPool = tomato::EntityPool<WaveColliderPoolTraits>;

#endif // !MANGO_WAVECOLLIDERPOOL_H
