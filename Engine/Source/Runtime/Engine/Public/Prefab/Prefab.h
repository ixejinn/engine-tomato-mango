#ifndef MANGO_PREFAB_H
#define MANGO_PREFAB_H

#include <string>
#include <entt/fwd.hpp>
#include <glm/vec3.hpp>
#include "Collision/CollisionConstants.h"

namespace tomato::Prefab
{
    /// Base components
    entt::entity CreateBaseEntity(
        entt::registry& registry,
        bool active = true, bool root = true,
        const std::string& name = "BaseEntity");

    /// Base + camera components
    entt::entity CreateCamera(
        entt::registry& registry,
        bool active = true, bool main = false,
        const std::string& name = "Camera",
        const glm::vec3& pos = {0.f, 1.f, 10.f}, const glm::vec3& rot = {0.f, 0.f, 0.f});

    /// Base + render components
    entt::entity CreateStaticMesh(
        entt::registry& registry,
        bool active = true,
        const std::string& name = "StaticMesh");

    /// Base components + trigger collider entity
    entt::entity CreateTriggerVolume(
        entt::registry& registry,
        bool active = true,
        const std::string& name = "TriggerVolume");

    /// Base + render components + (trigger) collider entity
    entt::entity CreateWorldObject(
        entt::registry& registry,
        bool active = true, bool trigger = false,
        bool printInfo = true,
        const std::string& name = "GameObject");

    /// Base + render + character base components + collider entity + ground trigger collider entity
    entt::entity CreateCharacter(
        entt::registry& registry,
        bool active = true,
        const std::string& name = "Character",
        bool printInfo = true);

    entt::entity AttachColliderEntity(
        entt::registry& registry, entt::entity parent,
        bool active = true, bool trigger = false,
        const std::string& name = "Collider");

    ///////// 구버전
    entt::entity CreateSkybox(entt::registry& reg);

    entt::entity CreateGizmo(entt::registry& reg);
}

#endif //MANGO_PREFAB_H
