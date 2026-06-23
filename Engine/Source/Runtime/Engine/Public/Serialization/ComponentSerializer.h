#ifndef MANGO_COMPONENTSERIALIZER_H
#define MANGO_COMPONENTSERIALIZER_H

#include <map>
#include <entt/fwd.hpp>
#include "Serialization/Json.h"
#include "UUID.h"

namespace tomato
{
	struct CameraComponent;
	struct TransformComponent;
	struct InputChannelComponent;
	struct MovementComponent;
	struct VelocityComponent;
	struct RenderComponent;

	struct UIComponent;
	struct CanvasComponent;
	struct RectTransformComponent;
	struct TextComponent;
	struct TargetComponent;
	struct MouseEventComponent;
	struct SelectableComponent;

	struct HierarchyComponent;
	struct RootEntityTag;
	struct MainCameraTag;
}

namespace tomato::Serialization
{
	void CreateJsonFile(const char*);
	json LoadJsonData(const char*);

	void SaveScene(entt::registry&, const char*);
	void LoadScene(entt::registry&, const char*);

	void SaveEntity(json&, entt::registry&, entt::entity);

	void CreateEntity(const json&, entt::registry&, std::map<UUID, entt::entity>&);
	void LoadComponents(const json&, entt::registry&, std::map<UUID, entt::entity>&);
	void LoadEntityComponents(const json&, entt::registry&, entt::entity);
	void ResolveHierarchy(entt::registry&, std::map<UUID, entt::entity>&);
	
	//Component Save & Load Func
	void Save(json&, const CameraComponent&);
	void Load(const json&, CameraComponent&);

	void Load(const json&, InputChannelComponent&);
	void Save(json&, const InputChannelComponent&);

	void Save(json&, const TransformComponent&);
	void Load(const json&, TransformComponent&);

	void Save(json&, const MovementComponent&);
	void Load(const json&, MovementComponent&);

	void Save(json&, const VelocityComponent&);
	void Load(const json&, VelocityComponent&);

	void Save(json&, const RenderComponent&);
	void Load(const json&, RenderComponent&);

	void Save(json&, const UIComponent&);
	void Load(const json&, UIComponent&);

	void Save(json&, const CanvasComponent&);
	void Load(const json&, CanvasComponent&);

	void Save(json&, const RectTransformComponent&);
	void Load(const json&, RectTransformComponent&);

	void Save(json&, const TextComponent&);
	void Load(const json&, TextComponent&);

	void Save(json&, const TargetComponent&);
	void Load(const json&, TargetComponent&);

	void Save(json&, const SelectableComponent&);
	void Load(const json&, SelectableComponent&);

	void Save(json&, const MouseEventComponent&);
	void Load(const json&, MouseEventComponent&);

	void Save(json&, const HierarchyComponent&);
	void Load(const json&, HierarchyComponent&);

	// For Tag
	void Save(json&, const RootEntityTag&);
	void Load(const json&, RootEntityTag&);

	void Save(json&, const MainCameraTag&);
	void Load(const json&, MainCameraTag&);
}

#endif // !MANGO_COMPONENTSERIALIZER_H
