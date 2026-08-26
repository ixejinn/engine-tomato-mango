#ifndef MANGO_ENTITY_H
#define MANGO_ENTITY_H

#include <string>
#include <unordered_map>
#include <entt/fwd.hpp>
#include "UUID.h"

namespace tomato
{
	bool ContainsUUID(entt::registry& reg, UUID id);
	bool ContainsName(entt::registry& reg, std::string_view name);

	entt::entity GetEntityByUUID(entt::registry& reg, UUID id);
	UUID GetUUID(entt::registry& reg, entt::entity e);

	bool IsVisible(entt::registry& reg, entt::entity e);

	void DestroyEntity(entt::registry& reg, entt::entity e);

	class EntityNameGenerator
	{
	public:
		//
		void Initialize(entt::registry& reg);
		std::string Generate(std::string_view baseName = "GameObject");

	private:
		// index to use next
		std::unordered_map<std::string, uint32_t> nextIndices_;
	};
}

#endif // !MANGO_ENTITY_H
