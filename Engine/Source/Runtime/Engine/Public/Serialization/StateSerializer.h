#ifndef MANGO_STATESERIALIZER_H
#define MANGO_STATESERIALIZER_H

#include <entt/fwd.hpp>
#include "Serialization/Json.h"
#include "Serialization/UUID.h"

namespace tomato::Serialization
{
	class StateSerializer
	{
	public:
		void SaveEntities(json& data, entt::registry& reg);
		void SaveEntity(json& data, entt::registry& reg, entt::entity entity);
		void CreateEntities(const json& root, entt::registry& reg, std::unordered_map<UUID, entt::entity>& entityMap);
		void LoadComponents(const json& root, entt::registry& reg, std::unordered_map<UUID, entt::entity>& entityMap);
		void LoadEntityComponents(const json& componentData, entt::registry& reg, entt::entity entity);
	};
}

#endif // !MANGO_STATESERIALIZER_H