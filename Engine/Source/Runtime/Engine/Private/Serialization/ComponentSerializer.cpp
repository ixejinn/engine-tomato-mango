#include <fstream>
#include <entt/entt.hpp>

//#include "Engine.h"

#include "State/State.h"
#include "State/StateRegistry.h"

#include "Serialization/ComponentSerializer.h"
#include "Serialization/ComponentRegistry.h"

#include "Resource/AssetRegistry.h"
#include "Resource/Render/Font.h"
#include "Resource/Render/Mesh.h"
#include "Resource/Render/Texture.h"
#include "Resource/Render/ParticleEffect.h"

#include "ECS/Components/Nametag.h"
#include "ECS/Components/Visibility.h"
#include "ECS/Components/Camera.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Rigidbody.h"
#include "ECS/Components/Collision.h"
#include "ECS/Components/Render.h"
#include "ECS/Components/UI.h"
#include "ECS/Components/UIEvents.h"
#include "ECS/Components/Text.h"
#include "ECS/Components/Target.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/Particle.h"

#include "Particle/ParticleEmitterPool.h"

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

	void LoadScene(entt::registry& reg, const char* path, std::unordered_map<UUID, entt::entity>& entityMap)
	{
		json root = LoadJsonData(path);
		if (root == nullptr)
			return;

		reg.clear();

		CreateEntity(root, reg, entityMap);

		LoadComponents(root, reg, entityMap);

		ResolveHierarchy(reg, entityMap);

		//AttachParticles(root, reg);
	}

	void SaveScene(State* state, const char* path)
	{
		json root;
		auto& reg = state->GetRegistry();

		root["State"] = StateRegistry::GetInstance().GetStateID(typeid(*state));
		root["State Name"] = std::type_index(typeid(*state)).name();

		SaveResourcesInfo(root);
		SaveParticlesInfo(root, reg);

		root["Entities"] = json::array();

		auto view = reg.view<NametagComponent>();
		for (auto entity : view)
		{
			json entityJson;

			SaveEntity(entityJson, reg, entity);
			if(!entityJson.empty())
				root["Entities"].push_back(entityJson);
		}

		std::ofstream ofs(path);
		ofs << root.dump(4);
	}

	void LoadStateScene(State* state, const char* path)
	{
		json root = LoadJsonData(path);
		if (root == nullptr)
			return;

		AssetID stateID = root["State"];

		auto newState = state->GetStateByID(stateID);
        auto& registry = newState->GetRegistry();

		//Load Resources
		LoadResources(root);

		CreateEntity(root, registry, newState->GetEntityMap());

		LoadComponents(root, registry, newState->GetEntityMap());

		ResolveHierarchy(registry, newState->GetEntityMap());

		AttachParticles(root, registry.ctx().get<ParticleEmitterPool>());

		state->SetNextState(std::move(newState));
	}

	void NewStateScene(State* state)
	{
		AssetID stateID = StateRegistry::GetInstance().GetStateID(typeid(*state));
		auto newState = state->GetStateByID(stateID);
		state->SetNextState(std::move(newState));
	}

	void LoadResources(const json& root)
	{
		for (auto& src : root["Resource"])
		{
			//std::cout << src.items().begin().key() << '\n';
			if (src.items().begin().key() == "Font")
			{
				for(auto& font : src["Font"])
					Font::Create(font);
			}

			if (src.items().begin().key() == "Mesh")
			{
				for (auto& mesh : src["Mesh"])
				{
					std::string meshName{ mesh };
					
					auto first = meshName.find("_");
					auto second = meshName.find("_", first + 1);

					auto sector = meshName.substr(first + 1, second - first - 1);
					auto stack = meshName.substr(second + 1);

					auto type = meshName.substr(0, first);
					for (auto meta : Mesh::PrimitiveMetas)
					{
						if (Mesh::GetPrimitiveName(meta.primitive) == type)
						{
							Mesh::Create(meta.primitive, std::stoi(sector), std::stoi(stack));
							break;
						}
					}
				}
			}

			if (src.items().begin().key() == "Texture")
			{
				for(auto& tex : src["Texture"])
					Texture::Create(tex);
			}

			if (src.items().begin().key() == "Particle")
			{
				for (auto& particle : src["Particle"])
					ParticleEffect::Create(particle);
			}
		}
	}

	void CreateEntity(const json& root, entt::registry& reg, std::unordered_map<UUID, entt::entity>& entityMap)
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

	void LoadComponents(const json& root, entt::registry& reg, std::unordered_map<UUID, entt::entity>& entityMap)
	{
		for (auto& entityData : root["Entities"])
		{
			UUID id = entityData["ID"];
			entt::entity e = entityMap[id];

			LoadEntityComponents(entityData["Components"], reg, e);
		}
	}

	void LoadEntityComponents(const json& componentData, entt::registry& reg, entt::entity entity)
	{
		for (auto& [key, value] : componentData.items())
		{
			//std::cout << "key : " << key << ", value : " << value << '\n';
			if (auto* info = ComponentRegistry::GetInstance().FindComponentInfo(key))
				info->serialization.Load(value, reg, entity);
		}
	}

	void ResolveHierarchy(entt::registry& reg, std::unordered_map<UUID, entt::entity>& entityMap)
	{
		auto view = reg.view<HierarchyComponent>();
		for (auto [e, hierarchy] : view.each())
		{
			hierarchy.parent =
				hierarchy.parentID == 0 ? entt::null : entityMap[hierarchy.parentID];
			for (auto child : hierarchy.childrenID)
				hierarchy.children.push_back(entityMap[child]);
		}
	}

	void AttachParticles(const json& particleData, ParticleEmitterPool& particlePool)
	{
		for (auto& particle : particleData["Particle"])
		{
			AssetID asset = particle["particle"];
			UUID target = particle["target"];

			particlePool.Acquire(asset, target);
		}
	}

	void SaveResourcesInfo(json& data)
	{
		data["Resource"] = json::array();

		json font, mesh, tex, particle;

		auto itBegin = AssetRegistry<Font>::GetInstance().GetNameMapBegin();
		auto itEnd = AssetRegistry<Font>::GetInstance().GetNameMapEnd();

		for (itBegin; itBegin != itEnd; ++itBegin)
			font["Font"].push_back(itBegin->second);
		
		data["Resource"].push_back(font);

		itBegin = AssetRegistry<Mesh>::GetInstance().GetNameMapBegin();
		itEnd = AssetRegistry<Mesh>::GetInstance().GetNameMapEnd();

		for (itBegin; itBegin != itEnd; ++itBegin)
		{
			bool flag = false;
			for (auto meta : Mesh::PrimitiveMetas)
			{
				if (Mesh::GetPrimitiveName(meta.primitive) == itBegin->second)
				{
					flag = true;
					break;
				}
			}

			if(!flag)
				mesh["Mesh"].push_back(itBegin->second);
		}

		data["Resource"].push_back(mesh);

		itBegin = AssetRegistry<Texture>::GetInstance().GetNameMapBegin();
		itEnd = AssetRegistry<Texture>::GetInstance().GetNameMapEnd();

		for (itBegin; itBegin != itEnd; ++itBegin)
			tex["Texture"].push_back(itBegin->second);

		data["Resource"].push_back(tex);

		itBegin = AssetRegistry<ParticleEffect>::GetInstance().GetNameMapBegin();
		itEnd = AssetRegistry<ParticleEffect>::GetInstance().GetNameMapEnd();

		for (itBegin; itBegin != itEnd; ++itBegin)
			particle["Particle"].push_back(itBegin->second);

		data["Resource"].push_back(particle);
	}

	void SaveParticlesInfo(json& data, entt::registry& reg)
	{
		data["Particle"] = json::array();
		
		auto view = reg.view<ParticleAttachmentComponent>();
		for (auto [e, attach] : view.each())
		{
			json particle;
			particle["particle"] = attach.particle;
			particle["target"] = attach.target;

			data["Particle"].push_back(particle);
		}
	}

	void SaveEntity(json& data, entt::registry& reg, entt::entity entity)
	{
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

	void SaveParticle(std::filesystem::path& path, entt::registry& reg, entt::entity entity)
	{
		json root;

		auto& componentInfo = ComponentRegistry::GetInstance().GetComponentInfo();
		for (auto& info : componentInfo)
		{
			if (info.category == Serialization::ComponentCategory::Particle)
				info.serialization.Save(root, reg, entity);

			/*if (info.name == "Target")
				info.serialization.Save(root, reg, entity);*/
		}

		std::ofstream ofs(path);
		ofs << root.dump(4);
	}

	void Save(json& data, const VisibilityComponent& visibility)
	{
		data["visible"] = visibility.visible;
	}

	void Load(const json& data, VisibilityComponent& visibility)
	{
		visibility.visible = data["visible"];
	}

	void Save(json& data, const CameraComponent& camera)
	{
		data["mode"] = camera.mode;
		data["degree"] = camera.degree;
		data["zNear"] = camera.zNear;
		data["zFar"] = camera.zFar;
	}

	void Load(const json& data, CameraComponent& camera)
	{
		camera.mode = data["mode"];
		camera.degree = data["degree"];
		camera.zNear = data["zNear"];
		camera.zFar = data["zFar"];
	}

	void Save(json& data, const InputChannelComponent& channel)
	{
		data["channel"] = channel.channel;
		data["1P"] = channel.is1P;
	}

	void Load(const json& data, InputChannelComponent& channel)
	{
		channel.channel = data["channel"];
		channel.is1P = data["1P"];
	}

	void Save(json& data, const TransformComponent& transform)
	{
		//TMT_DEBUG << "Save TransformComponent to Json";

		data["position"] = {
			transform.GetLocalPosition().x, transform.GetLocalPosition().y, transform.GetLocalPosition().z
		};

		data["degree"] = {
			transform.GetLocalRotationDegree().x, transform.GetLocalRotationDegree().y, transform.GetLocalRotationDegree().z
		};

		data["scale"] = {
			transform.GetLocalScale().x, transform.GetLocalScale().y, transform.GetLocalScale().z
		};
	}

	void Load(const json& data, TransformComponent& transform)
	{
		//TMT_DEBUG << "Load TransformComponent to Json";

		transform.SetPosition(
			glm::vec3(data["position"][0], data["position"][1], data["position"][2])
		);

		transform.SetRotationDegree(
			glm::vec3(data["degree"][0], data["degree"][1], data["degree"][2])
		);

		transform.SetScale(
			glm::vec3(data["scale"][0], data["scale"][1], data["scale"][2])
		);
	}

	void Save(json& data, const MovementComponent& movement)
	{
	}

	void Load(const json& data, MovementComponent& movement)
	{
	}

	void Save(json& data, const RenderComponent& render)
	{
		//TMT_DEBUG << "Save RenderComponent to Json";

		data["color"] = { render.color.x, render.color.y, render.color.z, render.color.w };
		data["mesh"] = render.mesh;
		data["shader"] = render.shader;
		data["texture"] = render.texture;
	}

	void Load(const json& data, RenderComponent& render)
	{
		//TMT_DEBUG << "Load RenderComponent to Json";

		render.color = {
			data["color"][0],
			data["color"][1],
			data["color"][2],
			data["color"][3]
		};

		render.mesh = data["mesh"];
		render.shader = data["shader"];
		render.texture = data["texture"];
	}

	void Save(json& data, const VelocityComponent& velocity)
	{
		data["speed"] = velocity.horizontalSpeed;
		data["velocity"] = { velocity.velocity.x, velocity.velocity.y, velocity.velocity.z };
	}

	void Load(const json& data, VelocityComponent& velocity)
	{
		velocity.horizontalSpeed = data["speed"];
		velocity.velocity =
		{
			data["velocity"][0],
			data["velocity"][1],
			data["velocity"][2],
		};
	}

	void Save(json& data, const ColliderComponent& collider)
	{
		data["layer"] = collider.layer;
		data["type"] =  collider.type;
		data["trigger"] = collider.isTrigger;
	}

	void Load(const json& data, ColliderComponent& collider)
	{
		collider.layer = data["layer"];
		collider.type = data["type"];
		collider.isTrigger = data["trigger"];
	}

	
	void Save(json& data, const UIComponent& ui)
	{
		data["canvas"] = ui.canvas;
		data["sortOrder"] = ui.sortOrder;
		data["type"] = ui.type;
	}

	void Load(const json& data, UIComponent& ui)
	{
		ui.canvas = data["canvas"];
		ui.sortOrder = data["sortOrder"];
		ui.type = data["type"];
	}

	void Save(json& data, const CanvasComponent& canvas)
	{
		data["refSize"] = { canvas.referenceSize.x, canvas.referenceSize.y };
		data["actSize"] = { canvas.actualSize.x, canvas.actualSize.y };
		data["sortOrder"] = canvas.sortOrder;
	}

	void Load(const json& data, CanvasComponent& canvas)
	{
		canvas.referenceSize = { data["refSize"][0], data["refSize"][1] };
		canvas.actualSize = { data["actSize"][0], data["actSize"][1] };
		canvas.sortOrder = data["sortOrder"];
	}

	void Save(json& data, const RectTransformComponent& rectTransform)
	{
		data["anchorPos"] = { rectTransform.anchoredPosition.x, rectTransform.anchoredPosition.y };
		data["offsetMin"] = { rectTransform.offsetMin.x, rectTransform.offsetMin.y };
		data["offsetMax"] = { rectTransform.offsetMax.x, rectTransform.offsetMax.y };
		data["sizeDelta"] = { rectTransform.sizeDelta.x, rectTransform.sizeDelta.y };
		data["anchorMin"] = { rectTransform.anchorMin.x, rectTransform.anchorMin.y };
		data["anchorMax"] = { rectTransform.anchorMax.x, rectTransform.anchorMax.y };
		data["pivot"] = { rectTransform.pivot.x, rectTransform.pivot.y };
	}

	void Load(const json& data, RectTransformComponent& rectTransform)
	{
		rectTransform.anchoredPosition = { data["anchorPos"][0], data["anchorPos"][1] };
		rectTransform.offsetMin = { data["offsetMin"][0], data["offsetMin"][1] };
		rectTransform.offsetMax = { data["offsetMax"][0], data["offsetMax"][1] };
		rectTransform.sizeDelta = { data["sizeDelta"][0], data["sizeDelta"][1] };
		rectTransform.anchorMin = { data["anchorMin"][0], data["anchorMin"][1] };
		rectTransform.anchorMax = { data["anchorMax"][0], data["anchorMax"][1] };
		rectTransform.pivot = { data["pivot"][0], data["pivot"][1] };
	}

	void Save(json& data, const TextComponent& text)
	{
		data["text"] = text.text;
		data["color"] = { text.color.x, text.color.y, text.color.z, text.color.w };
		data["fontSize"] = text.fontSize;
		data["font"] = text.font;
	}

	void Load(const json& data, TextComponent& text)
	{
		text.text = data["text"];
		text.color = {
			data["color"][0],
			data["color"][1],
			data["color"][2],
			data["color"][3],
		};
		text.fontSize = data["fontSize"];
		text.font = data["font"];
	}

	void Save(json& data, const TargetComponent& target)
	{
		data["target"] = target.target;
		data["offset"] = { target.headOffset.x, target.headOffset.y, target.headOffset.z };
	}

	void Load(const json& data, TargetComponent& target)
	{
		target.target = data["taget"];
		target.headOffset = {
			data["offset"][0],
			data["offset"][1],
			data["offset"][2]
		};
	}

	void Save(json& data, const MouseEventComponent& mouseEvt) {}
	void Load(const json& data, MouseEventComponent& mouseEvt) {}

	void Save(json& data, const SelectableComponent& selectable)
	{
		data["interactable"] = selectable.interactable;

		data["normal"] = { selectable.normalColor.x, selectable.normalColor.y, selectable.normalColor.z, selectable.normalColor.w };
		data["highlight"] = { selectable.highlightedColor.x, selectable.highlightedColor.y, selectable.highlightedColor.z, selectable.highlightedColor.w };
		data["pressed"] = { selectable.pressedColor.x, selectable.pressedColor.y, selectable.pressedColor.z, selectable.pressedColor.w };
	}

	void Load(const json& data, SelectableComponent& selectable)
	{
		selectable.interactable = data["interactable"];

		selectable.normalColor = {
			data["normal"][0],
			data["normal"][1],
			data["normal"][2],
			data["normal"][3]
		};

		selectable.highlightedColor = {
			data["highlight"][0],
			data["highlight"][1],
			data["highlight"][2],
			data["highlight"][3]
		};

		selectable.pressedColor = {
			data["pressed"][0],
			data["pressed"][1],
			data["pressed"][2],
			data["pressed"][3]
		};
	}

	void Save(json& data, const ParticleEmitterComponent& particle)
	{
		data["duration"] = std::chrono::duration<float>(particle.emitter.duration).count();
		data["looping"] = particle.looping;

		data["startDelay"] = std::chrono::duration<float>(particle.startDelay).count();
		data["startSpeed"] = particle.startSpeed;

		data["maxParticles"] = particle.maxParticles;
		data["shape"] = particle.shape;
		data["angle"] = particle.angle;
		data["space"] = particle.space;

		data["lifetime"] = std::chrono::duration<float>(particle.particleLifetime).count();
		data["rateOverTime"] = std::chrono::duration<float>(particle.emitPeriod).count();
		if (particle.burst.has_value())
		{
			data["burst"]["period"] = std::chrono::duration<float>(particle.burst.value().period).count();
			data["burst"]["cycles"] = particle.burst.value().cycles;
			data["burst"]["count"] = particle.burst.value().count;
		}
	}

	void Load(const json& data, ParticleEmitterComponent& particle) {}

	void Save(json& data, const ParticleRenderComponent& particle)
	{
		data["texture"] = particle.texture;
		data["size"] = particle.size;
		data["color"] = {
			particle.color.x,
			particle.color.y,
			particle.color.z,
			particle.color.w
		};
	}

	void Load(const json& data, ParticleRenderComponent& particle) {}

	void Save(json& data, const HierarchyComponent& hierarchy)
	{
		data["parent"] = hierarchy.parentID;
		data["children"] = hierarchy.childrenID;
	}

	void Load(const json& data, HierarchyComponent& hierarchy)
	{
		hierarchy.childrenID.clear();

		hierarchy.parentID = data["parent"];
		hierarchy.childrenID = data["children"].get<std::vector<UUID>>();
	}

	void Save(json& data, const RootEntityTag& rootTag) {}
	void Load(const json& data, RootEntityTag& rootTag) {}

	void Save(json& data, const MainCameraTag& camTag) {}
	void Load(const json& data, MainCameraTag& camTag) {}

	void Save(json& data, const CharacterTag& charTag) {}
	void Load(const json& data, CharacterTag& charTag) {}
}