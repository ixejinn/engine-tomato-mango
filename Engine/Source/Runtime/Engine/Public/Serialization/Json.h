#ifndef MANGO_JSON_H
#define MANGO_JSON_H

#include <json/json.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace tomato
{
	using json = nlohmann::ordered_json;
}

namespace nlohmann
{
	template<>
	struct adl_serializer<glm::vec2>
	{
		static void to_json(ordered_json& j, const glm::vec2& v)
		{
			j = json::array({v.x, v.y});
		}

		static void from_json(const ordered_json& j, glm::vec2& v)
		{
			v = {j.at(0).get<float>(), j.at(1).get<float>()};
		}
	};

	template<>
	struct adl_serializer<glm::vec3>
	{
		static void to_json(ordered_json& j, const glm::vec3& v)
		{
			j = json::array({v.x, v.y, v.z});
		}

		static void from_json(const ordered_json& j, glm::vec3& v)
		{
			v = {j.at(0).get<float>(), j.at(1).get<float>(), j.at(2).get<float>()};
		}
	};

	template<>
	struct adl_serializer<glm::vec4>
	{
		static void to_json(ordered_json& j, const glm::vec4& v)
		{
			j = json::array({v.x, v.y, v.z, v.w});
		}

		static void from_json(const ordered_json& j, glm::vec4& v)
		{
			v = {j.at(0).get<float>(), j.at(1).get<float>(), j.at(2).get<float>(), j.at(3).get<float>()};
		}
	};
}

#endif // !MANGO_JSON_H
