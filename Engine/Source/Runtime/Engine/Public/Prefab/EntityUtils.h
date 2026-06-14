#ifndef MANGO_ENTITYUTILS_H
#define MANGO_ENTITYUTILS_H

#include <string>
#include <entt/fwd.hpp>

#include "../../Core/Public/UUID.h"

namespace tomato
{
	bool ContainsUUID(entt::registry& reg, UUID id);
	bool ContainsName(entt::registry& reg, std::string_view name);
	std::string GenerateEntityName(entt::registry& reg);
}
#endif // !MANGO_ENTITYUTILS_H
