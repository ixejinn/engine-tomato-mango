#include <fstream>

#include "Serialization/ComponentSerializer.h"
#include "ECS/Components/Transform.h"

#include "Utils/Logger.h"

namespace tomato::Serialization
{
	void CreateJsonFile(const char* path)
	{
		std::ofstream ofs(path);
		ofs.close();
	}

	json LoadJsonData(const char* path)
	{
		std::ifstream jf(path);
		if (!jf.is_open())
		{
			TMT_INFO << "FILE NOT FOUND " << path;
			CreateJsonFile(path);

			return nullptr;
		}

		json data;
		jf.seekg(0, std::ios::end);

		if (jf.tellg() != 0)
		{
			jf.seekg(0);
			jf >> data;
		}

		jf.close();

		return data;
	}

	void Save(json& data, const TransformComponent& transform)
	{
		//TMT_DEBUG << "Save TransformComponent to Json";

		json transform_;
		transform_["position"]	= { transform.GetLocalPosition().x, transform.GetLocalPosition().y, transform.GetLocalPosition().z };
		transform_["degree"] = { transform.GetLocalEulerDegree().x, transform.GetLocalEulerDegree().y, transform.GetLocalEulerDegree().z };
		transform_["scale"] = { transform.GetLocalScale().x, transform.GetLocalScale().y, transform.GetLocalScale().z };

		data.push_back(transform_);
	}

	void Load(const json& data, TransformComponent& transform)
	{
		TMT_DEBUG << "Load TransformComponent to Json";

		auto transformData = data.find("Transform");
		if (transformData == data.end())
			return;
		
		glm::vec3 newValue;

		newValue.x = transformData.value().find("position").value().at(0);
		newValue.y = transformData.value().find("position").value().at(1);
		newValue.z = transformData.value().find("position").value().at(2);
		transform.SetPosition(newValue);

		newValue.x = transformData.value().find("degree").value().at(0);
		newValue.y = transformData.value().find("degree").value().at(1);
		newValue.z = transformData.value().find("degree").value().at(2);
		transform.SetEulerDegree(newValue);

		newValue.x = transformData.value().find("scale").value().at(0);
		newValue.y = transformData.value().find("scale").value().at(1);
		newValue.z = transformData.value().find("scale").value().at(2);
		transform.SetScale(newValue);
	}
}