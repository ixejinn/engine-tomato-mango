#ifndef MANGO_COMPONENTINFO_H
#define MANGO_COMPONENTINFO_H

#include <string>
#include <functional>
#include <entt/fwd.hpp>
#include "Serialization/Json.h"

namespace tomato
{
	struct EditorContext;
}

namespace tomato::Serialization
{
	using DrawInspector = std::function<void(EditorContext&, entt::registry&, entt::entity)>;
	struct ComponentInfo
	{
		std::string name;

		bool (*Has)(entt::registry&, entt::entity);
		void (*Save)(json&, entt::registry&, entt::entity);
		void (*Load)(const json&, entt::registry&, entt::entity);

		DrawInspector Draw;
	};
}
#endif // !MANGO_COMPONENTINFO_H
