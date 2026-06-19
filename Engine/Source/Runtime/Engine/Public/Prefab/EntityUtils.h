#ifndef MANGO_ENTITYUTILS_H
#define MANGO_ENTITYUTILS_H

#include <string>
#include <entt/fwd.hpp>

#include "UUID.h"

namespace tomato
{
	bool ContainsUUID(entt::registry& reg, UUID id);
	bool ContainsName(entt::registry& reg, std::string_view name);
	std::string GenerateEntityName(entt::registry& reg, std::string_view baseName = "Entity");
	
	entt::entity GetEntityByUUID(entt::registry& reg, UUID id);
	UUID GetUUID(entt::registry& reg, entt::entity e);
}
#endif // !MANGO_ENTITYUTILS_H
