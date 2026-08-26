#include <regex>
#include <entt/entt.hpp>

#include "ECS/Entity/Entity.h"
#include "ECS/Entity/Hierarchy.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/Nametag.h"
#include "ECS/Components/Visibility.h"

#include "Resource/AssetHash.h"

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

	entt::entity GetEntityByUUID(entt::registry& reg, UUID id)
	{
		if (id == 0)
			return entt::null;

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

	void EntityNameGenerator::Initialize(entt::registry& reg)
	{
		static const std::regex pattern(R"(^(.+) \((\d+)\)$)");
		nextIndices_.clear();

		auto view = reg.view<NametagComponent>();
		for (auto [e, name] : view.each())
		{
			std::smatch match;
			
			if (!std::regex_match(name.name, match, pattern))
				nextIndices_.try_emplace(GetAssetID(name.name.c_str()), 1);
			
			else
			{
				std::string baseName = match[1].str();
				uint32_t index = std::stoul(match[2].str());

				auto& nextIndex = nextIndices_[GetAssetID(baseName.c_str())];
				nextIndex = std::max(nextIndex, index + 1);
			}
		}
	}

	std::string EntityNameGenerator::Generate(std::string_view baseName)
	{
		auto id = GetAssetID(baseName.data());
		auto& nextIndex = nextIndices_[id];

		if (nextIndex == 0)
		{
			nextIndex = 1;
			return std::string(baseName);
		}

		return std::string(baseName) + " (" + std::to_string(nextIndex++) + ")";
	}
}