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
		collider.layer = data.value("layer", CollisionLayer::Default);
		collider.type = data.value("type", ColliderType::Cube);
		collider.trigger = data.value("trigger", false);

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
		hierarchy.root = GetEntityByUUID(registry, data["root"]);
		hierarchy.parent = GetEntityByUUID(registry, data["parent"]);

		hierarchy.children.clear();
		for (const auto& child : data["children"])
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
		channel.channel = data.value("channel", 0);
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
		physics.inverseMass = data.value("inverseMass", 1.0f);
	}

	// Render.h
	void Save(json& data, entt::registry&, const RenderComponent& render)
	{
		data["color"] = {render.color.r, render.color.g, render.color.b, render.color.a};
		data["mesh"] = render.mesh;
		data["shader"] = render.shader;
		data["texture"] = render.texture;
		data["priority"] = render.priority;
		data["doubleSided"] = render.doubleSided;
	}

	void Load(const json& data, entt::registry&, RenderComponent& render)
	{
		render.color =
		{
			data["color"][0],
			data["color"][1],
			data["color"][2],
			data["color"][3]
		};
		render.mesh = data["mesh"];
		render.shader = data["shader"];
		render.texture = data["texture"];
		render.priority = data.value("priority", RenderPriority::Opaque);
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
		data["offset"] = {target.headOffset.x, target.headOffset.y, target.headOffset.z};
	}

	void Load(const json& data, entt::registry& registry, TargetComponent& target)
	{
		target.target = GetEntityByUUID(registry, data["target"]);
		target.headOffset =
		{
			data["offset"][0],
			data["offset"][1],
			data["offset"][2]
		};
	}

	// Text.h
	void Save(json& data, entt::registry&, const TextComponent& text)
	{
		data["text"] = text.text;
		data["color"] = { text.color.x, text.color.y, text.color.z, text.color.w };
		data["font"] = text.font;
		data["fontSize"] = text.fontSize;
	}

	void Load(const json& data, entt::registry&, TextComponent& text)
	{
		text.text = data["text"];
		text.color = {
			data["color"][0],
			data["color"][1],
			data["color"][2],
			data["color"][3],
		};
		text.font = data["font"];
		text.fontSize = data.value("fontSize", 48.0f);

		text.dirty = true;
	}

	// Transform.h
	void Save(json& data, entt::registry&, const TransformComponent& transform)
	{
		auto position = transform.GetLocalPosition();
		data["position"] = {position.x, position.y, position.z};

		auto degree = transform.GetLocalRotationDegree();
		data["degree"] = {degree.x, degree.y, degree.z};

		auto scale = transform.GetLocalScale();
		data["scale"] = {scale.x, scale.y, scale.z};
	}

	void Load(const json& data, entt::registry&, TransformComponent& transform)
	{
		transform.SetPosition(data["position"][0], data["position"][1], data["position"][2]);
		transform.SetRotationDegree(data["degree"][0], data["degree"][1], data["degree"][2]);
		transform.SetScale(data["scale"][0], data["scale"][1], data["scale"][2]);
	}

	// UI.h
	void Save(json& data, entt::registry&, const CanvasComponent& canvas)
	{
		data["mode"] = canvas.mode;
		data["actSize"] = {canvas.actualSize.x, canvas.actualSize.y};
		data["refSize"] = {canvas.referenceSize.x, canvas.referenceSize.y};
		data["sortOrder"] = canvas.sortOrder;
	}

	void Load(const json& data, entt::registry&, CanvasComponent& canvas)
	{
		canvas.mode = data.value("mode", RenderMode::ScreenOverlay);
		canvas.actualSize = {data["actSize"][0], data["actSize"][1]};
		canvas.referenceSize = {data["refSize"][0], data["refSize"][1]};
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
		data["anchorPos"] = { rectTransform.anchoredPosition.x, rectTransform.anchoredPosition.y };
		data["offsetMin"] = { rectTransform.offsetMin.x, rectTransform.offsetMin.y };
		data["offsetMax"] = { rectTransform.offsetMax.x, rectTransform.offsetMax.y };
		data["sizeDelta"] = { rectTransform.sizeDelta.x, rectTransform.sizeDelta.y };
		data["anchorMin"] = { rectTransform.anchorMin.x, rectTransform.anchorMin.y };
		data["anchorMax"] = { rectTransform.anchorMax.x, rectTransform.anchorMax.y };
		data["pivot"] = { rectTransform.pivot.x, rectTransform.pivot.y };
	}

	void Load(const json& data, entt::registry&, RectTransformComponent& rectTransform)
	{
		rectTransform.anchoredPosition = {data["anchorPos"][0], data["anchorPos"][1]};
		rectTransform.offsetMin = {data["offsetMin"][0], data["offsetMin"][1]};
		rectTransform.offsetMax = {data["offsetMax"][0], data["offsetMax"][1]};
		rectTransform.sizeDelta = {data["sizeDelta"][0], data["sizeDelta"][1]};
		rectTransform.anchorMin = {data["anchorMin"][0], data["anchorMin"][1]};
		rectTransform.anchorMax = {data["anchorMax"][0], data["anchorMax"][1]};
		rectTransform.pivot = {data["pivot"][0], data["pivot"][1]};

		rectTransform.dirty = UI::Dirty::All;
	}

	void Save(json& data, entt::registry&, const SelectableComponent& selectable)
	{
		data["interactable"] = selectable.interactable;

		data["normal"] = {selectable.normalColor.x, selectable.normalColor.y, selectable.normalColor.z, selectable.normalColor.w};
		data["highlight"] = {selectable.highlightedColor.x, selectable.highlightedColor.y, selectable.highlightedColor.z, selectable.highlightedColor.w};
		data["pressed"] = {selectable.pressedColor.x, selectable.pressedColor.y, selectable.pressedColor.z, selectable.pressedColor.w};
	}

	void Load(const json& data, entt::registry&, SelectableComponent& selectable)
	{
		selectable.interactable = data["interactable"];

		selectable.normalColor =
		{
			data["normal"][0],
			data["normal"][1],
			data["normal"][2],
			data["normal"][3]
		};

		selectable.highlightedColor =
		{
			data["highlight"][0],
			data["highlight"][1],
			data["highlight"][2],
			data["highlight"][3]
		};

		selectable.pressedColor =
		{
			data["pressed"][0],
			data["pressed"][1],
			data["pressed"][2],
			data["pressed"][3]
		};
	}

	// Velocity.h
	void Save(json& data, entt::registry&, const VelocityComponent& velocity)
	{
		data["velocity"] = { velocity.velocity.x, velocity.velocity.y, velocity.velocity.z };
		data["speed"] = velocity.horizontalSpeed;
		data["acceleration"] = velocity.acceleration;
	}

	void Load(const json& data, entt::registry&, VelocityComponent& velocity)
	{
		velocity.velocity =
		{
			data["velocity"][0],
			data["velocity"][1],
			data["velocity"][2],
		};
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