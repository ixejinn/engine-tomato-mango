#ifndef MANGO_COMPONENTSERIALIZER_H
#define MANGO_COMPONENTSERIALIZER_H

#include <unordered_map>
#include <entt/fwd.hpp>
#include "Serialization/Json.h"
#include "UUID.h"
#include "State/StateFwd.h"
#include "ECS/Forward/ComponentFwd.h"
#include "Particle/ParticleEmitterPool.h"

namespace tomato
{
	class Engine;
}

namespace tomato::Serialization
{
	void CreateJsonFile(const char*);
	json LoadJsonData(const char*);

	/*void SaveScene(entt::registry&, const char*);
	void LoadScene(entt::registry&, const char*, std::unordered_map<UUID, entt::entity>&);*/
	
	void SaveScene(State*, const char*);
	void LoadStateScene(State*, const char*);
	void NewStateScene(State*);

	void SaveResourcesInfo(json&);
	void SaveParticlesInfo(json&, entt::registry&);
	void SaveEntity(json&, entt::registry&, entt::entity);

	void SaveParticle(std::filesystem::path&, entt::registry&, entt::entity);

	void LoadResources(const json&);
	void CreateEntity(const json&, entt::registry&, std::unordered_map<UUID, entt::entity>&);
	void LoadComponents(const json&, entt::registry&, std::unordered_map<UUID, entt::entity>&);
	void LoadEntityComponents(const json&, entt::registry&, entt::entity);
	void ResolveHierarchy(entt::registry&, std::unordered_map<UUID, entt::entity>&);
	void AttachParticles(const json&, entt::registry&);

	//Component Save & Load Func
	void Save(json&, entt::registry&, const VisibilityComponent&);
	void Load(const json&, entt::registry&, VisibilityComponent&);

	void Save(json&, entt::registry&, const CameraComponent&);
	void Load(const json&, entt::registry&, CameraComponent&);

	void Save(json&, entt::registry&, const InputChannelComponent&);
	void Load(const json&, entt::registry&, InputChannelComponent&);

	void Save(json&, entt::registry&, const TransformComponent&);
	void Load(const json&, entt::registry&, TransformComponent&);

	void Save(json&, entt::registry&, const MovementComponent&);
	void Load(const json&, entt::registry&, MovementComponent&);

	void Save(json&, entt::registry&, const VelocityComponent&);
	void Load(const json&, entt::registry&, VelocityComponent&);

	void Save(json&, entt::registry&, const ColliderComponent&);
	void Load(const json&, entt::registry&, ColliderComponent&);

	void Save(json&, entt::registry&, const RenderComponent&);
	void Load(const json&, entt::registry&, RenderComponent&);

	void Save(json&, entt::registry&, const UIComponent&);
	void Load(const json&, entt::registry&, UIComponent&);

	void Save(json&, entt::registry&, const CanvasComponent&);
	void Load(const json&, entt::registry&, CanvasComponent&);

	void Save(json&, entt::registry&, const RectTransformComponent&);
	void Load(const json&, entt::registry&, RectTransformComponent&);

	void Save(json&, entt::registry&, const TextComponent&);
	void Load(const json&, entt::registry&, TextComponent&);

	void Save(json&, entt::registry&, const TargetComponent&);
	void Load(const json&, entt::registry&, TargetComponent&);

	void Save(json&, entt::registry&, const SelectableComponent&);
	void Load(const json&, entt::registry&, SelectableComponent&);

	void Save(json&, entt::registry&, const MouseEventComponent&);
	void Load(const json&, entt::registry&, MouseEventComponent&);

	void Save(json&, entt::registry&, const ParticleEmitterComponent&);
	void Load(const json&, entt::registry&, ParticleEmitterComponent&);

	void Save(json&, entt::registry&, const ParticleRenderComponent&);
	void Load(const json&, entt::registry&, ParticleRenderComponent&);

	void Save(json&, entt::registry&, const HierarchyComponent&);
	void Load(const json&, entt::registry&, HierarchyComponent&);


	// For Tag
	void Save(json&, entt::registry&, const ActiveTag&);
	void Load(const json&, ActiveTag&);

	void Save(json&, entt::registry&, const RootEntityTag&);
	void Load(const json&, RootEntityTag&);

	void Save(json&, entt::registry&, const MainCameraTag&);
	void Load(const json&, MainCameraTag&);

	void Save(json&, entt::registry&, const CharacterTag&);
	void Load(const json&, CharacterTag&);

	void Save(json&, entt::registry&, const EditorHidden&);
	void Load(const json&, EditorHidden&);

	void Save(json&, entt::registry&, const NoInspector&);
	void Load(const json&, NoInspector&);
}

#endif // !MANGO_COMPONENTSERIALIZER_H
