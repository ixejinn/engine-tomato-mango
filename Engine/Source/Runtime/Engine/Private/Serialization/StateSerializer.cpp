#include <entt/entt.hpp>

#include "Serialization/StateSerializer.h"
#include "Serialization/ComponentRegistry.h"

#include "ECS/Components/Nametag.h"
#include "ECS/Components/Particle.h"
#include "ECS/Components/EditorTag.h"

namespace tomato::Serialization
{
	void StateSerializer::SaveEntities(json& root, entt::registry& reg)
	{
		auto view = reg.view<NametagComponent>();
		for (auto entity : view)
		{
			json entityJson;

			SaveEntity(entityJson, reg, entity);
			if (!entityJson.empty())
				root["Entities"].push_back(entityJson);
		}
	}

	void StateSerializer::SaveEntity(json& data, entt::registry& reg, entt::entity entity)
	{
		if (reg.all_of<NoSave>(entity))
			return;

		// particle
		if (reg.all_of<ParticleEmitterComponent>(entity))
			return;

		auto& tag = reg.get<NametagComponent>(entity);
		data["ID"] = tag.id;
		data["Name"] = tag.name;

		auto& componentInfo = ComponentRegistry::GetInstance().GetComponentInfo();

		json components = json::object();
		for (auto& info : componentInfo)
		{
			if (!info.serializable)
				continue;

			if (!info.Has(reg, entity))
				continue;

			if (info.category == Serialization::ComponentCategory::Particle)
				continue;

			json componentJson;
			info.serialization.Save(componentJson, reg, entity);

			components[info.name] = componentJson;
		}

		data["Components"] = components;
	}

	void StateSerializer::CreateEntities(const json& root, entt::registry& reg, std::unordered_map<UUID, entt::entity>& entityMap)
	{
		for (auto& entityData : root["Entities"])
		{
			UUID id = entityData["ID"];
			std::string name = entityData["Name"];

			entt::entity e = reg.create();
			reg.emplace<NametagComponent>(e, id, name);

			entityMap[id] = e;
		}
	}

	void StateSerializer::LoadComponents(const json& root, entt::registry& reg, std::unordered_map<UUID, entt::entity>& entityMap)
	{
		for (auto& entityData : root["Entities"])
		{
			UUID id = entityData["ID"];
			entt::entity e = entityMap[id];

			LoadEntityComponents(entityData["Components"], reg, e);
		}
	}

	void StateSerializer::LoadEntityComponents(const json& componentData, entt::registry& reg, entt::entity entity)
	{
		for (auto& [key, value] : componentData.items())
		{
			//std::cout << "key : " << key << ", value : " << value << '\n';
			if (auto* info = ComponentRegistry::GetInstance().FindComponentInfo(key))
				info->serialization.Load(value, reg, entity);
		}
	}
}