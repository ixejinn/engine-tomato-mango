#ifndef MANGO_COMPONENTINFO_H
#define MANGO_COMPONENTINFO_H

#include <string>
#include <entt/fwd.hpp>
#include "Serialization/Json.h"

namespace tomato::Serialization
{
	struct ComponentInfo
	{
		std::string name;

		bool (*Has)(entt::registry&, entt::entity);
		void (*Save)(json&, entt::registry&, entt::entity);
		void (*Load)(const json&, entt::registry&, entt::entity);
	};
}
#endif // !MANGO_COMPONENTINFO_H
