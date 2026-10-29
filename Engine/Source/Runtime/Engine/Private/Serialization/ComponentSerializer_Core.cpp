#include "Serialization/ComponentSerializer.h"

#include "ECS/Components/Nametag.h"
#include "ECS/Components/Visibility.h"
#include "ECS/Components/Camera.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Velocity.h"
#include "ECS/Components/Collision.h"
#include "ECS/Components/PhysicsBody.h"
#include "ECS/Components/Render.h"
#include "ECS/Components/Target.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/ActiveTag.h"
#include "ECS/Components/EditorTag.h"

namespace tomato::Serialization
{
	void Save(json& data, entt::registry& registry, const VisibilityComponent& visibility)
	{
		data["visible"] = visibility.visible;
	}

	void Load(const json& data, entt::registry& registry, VisibilityComponent& visibility)
	{
		visibility.visible = data["visible"];
	}

	void Save(json& data, entt::registry& registry, const CameraComponent& camera)
	{
		data["mode"] = camera.mode;
		data["degree"] = camera.degree;
		data["zNear"] = camera.zNear;
		data["zFar"] = camera.zFar;
	}

	void Load(const json& data, entt::registry& registry, CameraComponent& camera)
	{
		camera.mode = data["mode"];
		camera.degree = data["degree"];
		camera.zNear = data["zNear"];
		camera.zFar = data["zFar"];
	}

	void Save(json& data, entt::registry& registry, const InputChannelComponent& channel)
	{
		data["channel"] = channel.channel;
		data["key Setting"] = channel.useWASD;
	}

	void Load(const json& data, entt::registry& registry, InputChannelComponent& channel)
	{
		channel.channel = data["channel"];
		channel.useWASD = data["key Setting"];
	}

	void Save(json& data, entt::registry& registry, const TransformComponent& transform)
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

	void Load(const json& data, entt::registry& registry, TransformComponent& transform)
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

	void Save(json& data, entt::registry& registry, const MovementComponent& movement)
	{
	}

	void Load(const json& data, entt::registry& registry, MovementComponent& movement)
	{
	}

	void Save(json& data, entt::registry& registry, const RenderComponent& render)
	{
		//TMT_DEBUG << "Save RenderComponent to Json";

		data["color"] = { render.color.x, render.color.y, render.color.z, render.color.w };
		data["mesh"] = render.mesh;
		data["shader"] = render.shader;
		data["texture"] = render.texture;
	}

	void Load(const json& data, entt::registry& registry, RenderComponent& render)
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

	void Save(json& data, entt::registry& registry, const VelocityComponent& velocity)
	{
		data["speed"] = velocity.horizontalSpeed;
		data["velocity"] = { velocity.velocity.x, velocity.velocity.y, velocity.velocity.z };
	}

	void Load(const json& data, entt::registry& registry, VelocityComponent& velocity)
	{
		velocity.horizontalSpeed = data["speed"];
		velocity.velocity =
		{
			data["velocity"][0],
			data["velocity"][1],
			data["velocity"][2],
		};
	}

	void Save(json& data, entt::registry& registry, const ColliderComponent& collider)
	{
		data["layer"] = collider.layer;
		data["type"] = collider.type;
		data["trigger"] = collider.trigger;
	}

	void Load(const json& data, entt::registry& registry, ColliderComponent& collider)
	{
		collider.layer = data["layer"];
		collider.type = data["type"];
		collider.trigger = data["trigger"];
	}

	void Save(json& data, entt::registry& registry, const PhysicsBodyComponent& physics)
	{
		data["inverse"] = physics.inverseMass;
	}

	void Load(const json& data, entt::registry& registry, PhysicsBodyComponent& physics)
	{
		physics.inverseMass = data["inverse"];
	}

	void Save(json& data, entt::registry& registry, const TargetComponent& target)
	{
		data["target"] = GetUUID(registry, target.target);
		data["offset"] = { target.headOffset.x, target.headOffset.y, target.headOffset.z };
	}

	void Load(const json& data, entt::registry& registry, TargetComponent& target)
	{
		target.target = GetEntityByUUID(registry, data["target"]);
		target.headOffset = {
			data["offset"][0],
			data["offset"][1],
			data["offset"][2]
		};
	}

	void Save(json& data, entt::registry& registry, const HierarchyComponent& hierarchy)
	{
		data["root"] = GetUUID(registry, hierarchy.root);
		data["parent"] = GetUUID(registry, hierarchy.parent);

		data["children"] = nlohmann::json::array();
		for (auto child : hierarchy.children)
			data["children"].push_back(GetUUID(registry, child));
	}

	void Load(const json& data, entt::registry& registry, HierarchyComponent& hierarchy)
	{
		hierarchy.children.clear();

		hierarchy.root = GetEntityByUUID(registry, data["root"]);
		hierarchy.parent = GetEntityByUUID(registry, data["parent"]);

		for (const auto& child : data["children"])
			hierarchy.children.push_back(GetEntityByUUID(registry, child));
	}

	// Tag
	void Save(json& data, entt::registry& registry, const ActiveTag& activeTag) {}
	void Load(const json& data, entt::registry& registry, ActiveTag& activeTag) {}

	void Save(json& data, entt::registry& registry, const RootEntityTag& rootTag) {}
	void Load(const json& data, entt::registry& registry, RootEntityTag& rootTag) {}

	void Save(json& data, entt::registry& registry, const MainCameraTag& camTag) {}
	void Load(const json& data, entt::registry& registry, MainCameraTag& camTag) {}

	void Save(json& data, entt::registry& registry, const CharacterTag& charTag) {}
	void Load(const json& data, entt::registry& registry, CharacterTag& charTag) {}

	void Save(json& data, entt::registry& registry, const EditorHidden& eHiddenTag) {}
	void Load(const json& data, entt::registry& registry, EditorHidden&) {}

	void Save(json& data, entt::registry& registry, const NoInspector& noInspectorTag) {}
	void Load(const json& data, entt::registry& registry, NoInspector& noInspectorTag) {}
}