#ifndef MANGO_ENTITY_H
#define MANGO_ENTITY_H

#include <string>
#include <unordered_map>
#include <entt/fwd.hpp>
#include "UUID.h"
#include "Resource/ResourceFwd.h"
#include "ECS/Forward/EntityCompFwd.h"

namespace tomato
{
	entt::entity GetEntityByUUID(entt::registry& reg, UUID id);
	UUID GetUUID(entt::registry& reg, entt::entity e);

    bool IsVisible(const VisibilityComponent& visibility);
	bool IsVisible(entt::registry& reg, entt::entity e);

    void OnVisibilityComponentUpdated(entt::registry& registry, entt::entity e);
    void OnPendingDestroyComponentConstructed(entt::registry& registry, entt::entity e);
    void OnActiveTagDestroyed(entt::registry& registry, entt::entity e);

	class EntityNameGenerator
	{
	public:
		//
		void Initialize(entt::registry& reg);
		std::string Generate(std::string_view baseName = "GameObject");

	private:
		// index to use next
		std::unordered_map<AssetID, uint32_t> nextIndices_;
	};
}

#endif // !MANGO_ENTITY_H
