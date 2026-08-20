#ifndef MANGO_PREFAB_H
#define MANGO_PREFAB_H

#include <entt/fwd.hpp>
#include <glm/vec3.hpp>
#include "Collision/CollisionConstants.h"

namespace tomato::Prefab
{
    enum Primitive
    {
        Cube,
        Sphere
    };

    entt::entity CreateEmpty(
            entt::registry& reg,
            Primitive mesh = Cube,
            glm::vec3 pos = glm::vec3{ 0.f });

    entt::entity CreateStaticObject(
            entt::registry& reg,
            Primitive mesh = Cube,
            glm::vec3 pos = glm::vec3{0.f});
   
    entt::entity CreateCharacter(
            entt::registry& reg,
            Primitive mesh = Cube,
            glm::vec3 pos = glm::vec3{0.f});

    entt::entity CreateCamera(
            entt::registry& reg,
            bool isMain = true,
            glm::vec3 pos = {0.f, 1.f, 10.f},
            glm::vec3 rot = {0.f, 0.f, 0.f});

    entt::entity CreateSkybox(entt::registry& reg);

    entt::entity CreateGizmo(entt::registry& reg);

    entt::entity AttachCollider(entt::registry& reg, entt::entity parent, ColliderType type);

    entt::entity AttachCharacterCollider(entt::registry& reg, entt::entity parent, ColliderType type);
}

#endif //MANGO_PREFAB_H
