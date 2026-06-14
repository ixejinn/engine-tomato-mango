#ifndef MANGO_COMPONENTSERIALIZER_H
#define MANGO_COMPONENTSERIALIZER_H

#include <json/json.hpp>

namespace tomato
{
	struct TransformComponent;
}

namespace tomato::Serialization
{
	using json = nlohmann::json;

	void CreateJsonFile(const char*);
	json LoadJsonData(const char*);

	void Save(json&, const TransformComponent&);
	void Load(const json&, TransformComponent&);
}

#endif // !MANGO_COMPONENTSERIALIZER_H
