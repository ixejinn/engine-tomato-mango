#ifndef MANGO_PREFAB_H
#define MANGO_PREFAB_H

#include <string>
#include <entt/fwd.hpp>
#include <glm/vec3.hpp>
#include "Collision/CollisionConstants.h"

namespace tomato::Prefab
{
    entt::entity CreateBaseEntity(
        entt::registry& registry, const std::string& name = "BaseEntity", bool root = true);

    entt::entity CreateCamera(
        entt::registry& registry, const std::string& name = "Camera",
        bool main = false, const glm::vec3& pos = {0.f, 1.f, 10.f}, const glm::vec3& rot = {0.f, 0.f, 0.f});

    entt::entity CreateStaticMesh(entt::registry& registry, const std::string& name = "StaticMesh");

    entt::entity CreateTriggerVolume(entt::registry& registry, const std::string& name = "TriggerVolume");

    entt::entity CreateWorldObject(entt::registry& registry, const std::string& name = "GameObject", bool printInfo = false);

    entt::entity CreateCharacter(entt::registry& registry, const std::string& name = "Character", bool printInfo = false);

    entt::entity AttachColliderEntity(entt::registry& registry, entt::entity parent, bool trigger, const std::string& name = "Collider");

    ///////// 구버전
    entt::entity CreateSkybox(entt::registry& reg);

    entt::entity CreateGizmo(entt::registry& reg);
}

#endif //MANGO_PREFAB_H
