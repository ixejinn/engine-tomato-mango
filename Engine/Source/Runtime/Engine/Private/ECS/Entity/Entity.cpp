#include <entt/entt.hpp>

#include "ECS/Entity/Entity.h"
#include "ECS/Entity/Hierarchy.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/Nametag.h"
#include "ECS/Components/Visibility.h"

namespace tomato
{
	bool ContainsUUID(entt::registry& reg, UUID id)
	{
		auto view = reg.view<NametagComponent>();
		for (auto [e, tag] : view.each())
			return tag.id == id;

		return false;
	}

	bool ContainsName(entt::registry& reg, std::string_view name)
	{
		auto view = reg.view<NametagComponent>();
		for (auto [e, tag] : view.each())
			return tag.name == name;

		return false;
	}

	std::string GenerateEntityName(entt::registry& reg, std::string_view baseName)
	{
		auto view = reg.view<NametagComponent>();

		auto Exists = [&](const std::string& name)
			{
				for (auto [e, tag] : view.each())
				{
					if (tag.name == name)
						return true;
				}
				return false;
			};

		std::string candidate(baseName);

		if (!Exists(candidate))
			return candidate;

		int index = 1;

		while (true)
		{
			candidate = std::string(baseName) + " (" + std::to_string(index) + ")";

			if (!Exists(candidate))
				return candidate;

			++index;
		}
	}

	entt::entity GetEntityByUUID(entt::registry& reg, UUID id)
	{
		const auto view = reg.view<NametagComponent>();
		for (auto [e, tag] : view.each())
		{
			if (tag.id == id)
				return e;
		}

		return entt::null;
	}

	UUID GetUUID(entt::registry& reg, entt::entity e)
	{
		auto* tag = reg.try_get<NametagComponent>(e);

		return e == entt::null ? 0 : tag->id;
	}

	bool IsVisible(entt::registry& reg, entt::entity e)
	{
		auto* v = reg.try_get<VisibilityComponent>(e);
		if (v)
			return v->visible && v->inheritedVisible;

		throw std::runtime_error("Not found Visibility Component");
	}

    void DestroyEntity(entt::registry& reg, entt::entity e) {
        if (reg.try_get<HierarchyComponent>(e))
            DestroyHierarchyEntity(reg, e);
        else
            reg.destroy(e);
    }
}