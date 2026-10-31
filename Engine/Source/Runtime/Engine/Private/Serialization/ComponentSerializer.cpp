#include "Serialization/ComponentSerializer.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/UIDirty.h"

namespace tomato::Serialization
{
	// Camera.h
	void Save(json& data, entt::registry&, const CameraComponent& camera)
	{
		data["mode"] = camera.mode;
		data["degree"] = camera.degree;
		data["zNear"] = camera.zNear;
		data["zFar"] = camera.zFar;
	}

	void Load(const json& data, entt::registry&, CameraComponent& camera)
	{
		camera.mode = data.value("mode", Perspective);
		camera.degree = data.value("degree", 45.0f);
		camera.zNear = data.value("zNear", 0.1f);
		camera.zFar = data.value("zFar", 100.0f);

		camera.dirty = true;
	}

	// Collision.h
	void Save(json& data, entt::registry&, const ColliderComponent& collider)
	{
		data["layer"] = collider.layer;
		data["type"] = collider.type;
		data["trigger"] = collider.trigger;
	}

	void Load(const json& data, entt::registry&, ColliderComponent& collider)
	{
		collider.layer = data.at("layer").get<CollisionLayer>();
		collider.type = data.at("type").get<ColliderType>();
		collider.trigger = data.at("trigger").get<bool>();

		collider.aabbDirty = true;
	}

	// Hierarchy.h
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
		hierarchy.root = GetEntityByUUID(registry, data.at("root").get<UUID>());
		hierarchy.parent = GetEntityByUUID(registry, data.at("parent").get<UUID>());

		const auto uuids = data.at("children").get<std::vector<UUID>>();
		hierarchy.children.clear();
		hierarchy.children.reserve(uuids.size());
		for (const auto child : uuids)
			hierarchy.children.push_back(GetEntityByUUID(registry, child));
	}

	// Movement.h
	void Save(json& data, entt::registry&, const InputChannelComponent& channel)
	{
		data["channel"] = channel.channel;
		data["useWASD"] = channel.useWASD;
	}

	void Load(const json& data, entt::registry&, InputChannelComponent& channel)
	{
		channel.channel = data.at("channel").get<uint8_t>();
		channel.useWASD = data.value("useWASD", true);
	}

	void Save(json& data, entt::registry&, const MovementComponent& movement)
	{
		data["control"] = movement.control;
	}

	void Load(const json& data, entt::registry&, MovementComponent& movement)
	{
		movement.control = data.value("control", Direct);
	}

	// PhysicsBody.h
	void Save(json& data, entt::registry&, const PhysicsBodyComponent& physics)
	{
		data["inverseMass"] = physics.inverseMass;
	}

	void Load(const json& data, entt::registry&, PhysicsBodyComponent& physics)
	{
		physics.inverseMass = data.at("inverseMass").get<float>();
	}

	// Render.h
	void Save(json& data, entt::registry&, const RenderComponent& render)
	{
		data["color"] = render.color;
		data["mesh"] = render.mesh;
		data["shader"] = render.shader;
		data["texture"] = render.texture;
		data["priority"] = render.priority;
		data["doubleSided"] = render.doubleSided;
	}

	void Load(const json& data, entt::registry&, RenderComponent& render)
	{
		render.color = data["color"];
		render.mesh = data.at("mesh").get<AssetID>();
		render.shader = data.at("shader").get<AssetID>();
		render.texture = data.at("texture").get<AssetID>();
		render.priority = data.at("priority").get<RenderPriority>();
		render.doubleSided = data.value("doubleSided", false);
	}

	void Save(json& data, entt::registry&, const StencilComponent& stencil)
	{
		data["func"] = stencil.func;
		data["writeRef"] = stencil.writeRef;
		data["readRef"] = stencil.readRef;
	}

	void Load(const json& data, entt::registry&, StencilComponent& stencil)
	{
		stencil.func = data.value("func", GL_ALWAYS);
		stencil.writeRef = data.value("writeRef", 0);
		stencil.readRef = data.value("readRef", 0);
	}

	// Target.h
	void Save(json& data, entt::registry& registry, const TargetComponent& target)
	{
		data["target"] = GetUUID(registry, target.target);
		data["offset"] = target.headOffset;
	}

	void Load(const json& data, entt::registry& registry, TargetComponent& target)
	{
		target.target = GetEntityByUUID(registry, data["target"]);
		target.headOffset = data["offset"];
	}

	// Text.h
	void Save(json& data, entt::registry&, const TextComponent& text)
	{
		data["text"] = text.text;
		data["color"] = text.color;
		data["font"] = text.font;
		data["fontSize"] = text.fontSize;
	}

	void Load(const json& data, entt::registry&, TextComponent& text)
	{
		text.text = data["text"];
		text.color = data["color"];
		text.font = data["font"];
		text.fontSize = data.value("fontSize", 48.0f);

		text.dirty = true;
	}

	// Transform.h
	void Save(json& data, entt::registry&, const TransformComponent& transform)
	{
		data["position"] = transform.GetLocalPosition();
		data["rotation"] = transform.GetLocalRotationDegree();
		data["scale"] = transform.GetLocalScale();
	}

	void Load(const json& data, entt::registry&, TransformComponent& transform)
	{
		const glm::vec3 position = data["position"];
		const glm::vec3 rotation = data["rotation"];
		const glm::vec3 scale = data["scale"];

		transform.SetPosition(position);
		transform.SetRotationDegree(rotation);
		transform.SetScale(scale);
	}

	// UI.h
	void Save(json& data, entt::registry&, const CanvasComponent& canvas)
	{
		data["mode"] = canvas.mode;
		data["actSize"] = canvas.actualSize;
		data["refSize"] = canvas.referenceSize;
		data["sortOrder"] = canvas.sortOrder;
	}

	void Load(const json& data, entt::registry&, CanvasComponent& canvas)
	{
		canvas.mode = data.value("mode", RenderMode::ScreenOverlay);
		canvas.actualSize = data["actSize"];
		canvas.referenceSize = data["refSize"];
		canvas.sortOrder = data.value("sortOrder", 0);
	}

	void Save(json& data, entt::registry& registry, const UIComponent& ui)
	{
		data["canvas"] = GetUUID(registry, ui.canvas);
		data["sortOrder"] = ui.sortOrder;
		data["type"] = ui.type;
	}

	void Load(const json& data, entt::registry& registry, UIComponent& ui)
	{
		ui.canvas = GetEntityByUUID(registry, data["canvas"]);
		ui.sortOrder = data.value("sortOrder", 0);
		ui.type = data.value("type", UIType::Default);
	}

	void Save(json& data, entt::registry&, const RectTransformComponent& rectTransform)
	{
		data["anchorPos"] = rectTransform.anchoredPosition;
		data["offsetMin"] = rectTransform.offsetMin;
		data["offsetMax"] = rectTransform.offsetMax;
		data["sizeDelta"] = rectTransform.sizeDelta;
		data["anchorMin"] = rectTransform.anchorMin;
		data["anchorMax"] = rectTransform.anchorMax;
		data["pivot"] = rectTransform.pivot;
	}

	void Load(const json& data, entt::registry&, RectTransformComponent& rectTransform)
	{
		rectTransform.anchoredPosition = data["anchorPos"];
		rectTransform.offsetMin = data["offsetMin"];
		rectTransform.offsetMax = data["offsetMax"];
		rectTransform.sizeDelta = data["sizeDelta"];
		rectTransform.anchorMin = data["anchorMin"];
		rectTransform.anchorMax = data["anchorMax"];
		rectTransform.pivot = data["pivot"];

		rectTransform.dirty = UI::Dirty::All;
	}

	void Save(json& data, entt::registry&, const SelectableComponent& selectable)
	{
		data["interactable"] = selectable.interactable;

		data["normal"] = selectable.normalColor;
		data["highlight"] = selectable.highlightedColor;
		data["pressed"] = selectable.pressedColor;
	}

	void Load(const json& data, entt::registry&, SelectableComponent& selectable)
	{
		selectable.interactable = data["interactable"];

		selectable.normalColor = data["normal"];
		selectable.highlightedColor = data["highlight"];
		selectable.pressedColor = data["pressed"];
	}

	// Velocity.h
	void Save(json& data, entt::registry&, const VelocityComponent& velocity)
	{
		data["velocity"] = velocity.velocity;
		data["speed"] = velocity.horizontalSpeed;
		data["acceleration"] = velocity.acceleration;
	}

	void Load(const json& data, entt::registry&, VelocityComponent& velocity)
	{
		velocity.velocity = data["velocity"];
		velocity.horizontalSpeed = data.value("speed", 3.0f);
		velocity.acceleration = data.value("acceleration", 20.0f);
	}

	void Save(json& data, entt::registry&, const FrictionComponent& friction)
	{
		data["groundFriction"] = friction.groundFriction;
		data["airDrag"] = friction.airDrag;
	}

	void Load(const json& data, entt::registry&, FrictionComponent& friction)
	{
		friction.groundFriction = data.value("groundFriction", 5.0f);
		friction.airDrag = data.value("airDrag", 0.5f);
	}

	void Save(json& data, entt::registry&, const SurfaceComponent& surface)
	{
		data["friction"] = surface.friction;
	}

	void Load(const json& data, entt::registry&, SurfaceComponent& surface)
	{
		surface.friction = data.value("friction", 1.0f);
	}

	// Visibility.h
	void Save(json& data, entt::registry&, const VisibilityComponent& visibility)
	{
		data["visible"] = visibility.visible;
	}

	void Load(const json& data, entt::registry&, VisibilityComponent& visibility)
	{
		visibility.visible = data.value("visible", true);
	}
}