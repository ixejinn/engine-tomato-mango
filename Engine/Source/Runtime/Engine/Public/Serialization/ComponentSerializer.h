#ifndef MANGO_COMPONENTSERIALIZER_H
#define MANGO_COMPONENTSERIALIZER_H

#include <entt/fwd.hpp>
#include "Serialization/Json.h"

namespace tomato
{
	struct TransformComponent;
	struct RenderComponent;
	struct NametagComponent;
}

namespace tomato::Serialization
{
	void CreateJsonFile(const char*);
	json LoadJsonData(const char*);

	void SaveScene(entt::registry&, const char*);

	void SaveEntity(json&, entt::registry&, entt::entity);

	void Save(json&, const NametagComponent&);
	void Load(const json&, NametagComponent&);

	void Save(json&, const TransformComponent&);
	void Load(const json&, TransformComponent&);

	void Save(json&, const RenderComponent&);
	void Load(const json&, RenderComponent&);
}

#endif // !MANGO_COMPONENTSERIALIZER_H
