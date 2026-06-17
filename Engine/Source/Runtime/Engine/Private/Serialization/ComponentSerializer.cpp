#include <fstream>
#include <entt/entt.hpp>

#include "Serialization/ComponentSerializer.h"
#include "Serialization/ComponentRegistry.h"

#include "ECS/Components/Nametag.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Render.h"

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

	void SaveScene(entt::registry& reg, const char* path)
	{
		json root;

		root["Entities"] = json::array();

		auto view = reg.view<NametagComponent>();
		for (auto entity : view)
		{
			json entityJson;

			SaveEntity(entityJson, reg, entity);

			root["Entities"].push_back(entityJson);
		}

		std::ofstream ofs(path);
		ofs << root.dump(4);
	}

	void SaveEntity(json& data, entt::registry& reg, entt::entity entity)
	{
		auto& tag = reg.get<NametagComponent>(entity);
		data["ID"] = tag.id;
		data["Name"] = tag.name;

		json components = json::object();
#if 0
		if (reg.any_of<TransformComponent>(entity))
		{
			json transform;
			Save(transform, reg.get<TransformComponent>(entity));

			components["Transform"] = transform;
		}
#elif 1
		auto& componentInfo = ComponentRegistry::GetInstance().GetInfo();
		for (auto& info : componentInfo)
		{
			if (!info.Has(reg, entity))
				continue;

			json componentJson;
			info.Save(componentJson, reg, entity);

			components[info.name] = componentJson;
		}
#endif
		data["Components"] = components;
	}

	void Save(json& data, const NametagComponent& nametag)
	{
		//TMT_DEBUG << "Save NametagComponent to Json";

		json nametag_;
		nametag_["id"] = nametag.id;
		nametag_["name"] = nametag.name;

		data.push_back(nametag_);
	}

	void Load(const json& data, NametagComponent& nametag)
	{
		auto idData = data.find("Nametag");
		if (idData == data.end())
			return;

		nametag.id = idData.value().find("id").value();
		nametag.name = idData.value().find("name").value();
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
		//TMT_DEBUG << "Load TransformComponent to Json";

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

	void Save(json& data, const RenderComponent& render)
	{
		//TMT_DEBUG << "Save RenderComponent to Json";

		json render_;
		render_["color"] = { render.color.x, render.color.y, render.color.z, render.color.w };
		render_["mesh"] = render.mesh;
		render_["shader"] = render.shader;
		render_["texture"] = render.texture;
		
		data.push_back(render_);
	}

	void Load(const json& data, RenderComponent& render)
	{
		//TMT_DEBUG << "Load RenderComponent to Json";

		auto renderData = data.find("Render");
		if (renderData == data.end())
			return;

		glm::vec4 inColor;
		inColor.x = renderData.value().find("color").value().at(0);
		inColor.y = renderData.value().find("color").value().at(1);
		inColor.z = renderData.value().find("color").value().at(2);
		inColor.w = renderData.value().find("color").value().at(3);

		render.color = inColor;

		render.mesh = renderData.value().find("mesh").value();
		render.shader = renderData.value().find("shader").value();
		render.texture = renderData.value().find("texture").value();
	}
}