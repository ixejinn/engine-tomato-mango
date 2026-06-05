#ifndef MANGO_PREFAB_H
#define MANGO_PREFAB_H

#include <entt/fwd.hpp>
#include <glm/vec3.hpp>
#include "Collision/CollisionConstants.h"

namespace tomato {
    struct Prefab {
        enum Primitive {
            Cube,
            Sphere
        };

        static entt::entity CreateStaticObject(entt::registry& reg,
                                               Primitive mesh = Cube,
                                               glm::vec3 pos = glm::vec3{0.f});
        static entt::entity CreateCharacter(entt::registry& reg,
                                            Primitive mesh = Cube,
                                            glm::vec3 pos = glm::vec3{0.f});

        static entt::entity CreateCamera(entt::registry& reg,
                                         glm::vec3 pos = {0.f, 7.5f, 15.f},
                                         glm::vec3 rot = {-30.f, 0.f, 0.f},
                                         bool isMain = true);

        static entt::entity AttachCollider(entt::registry& reg, entt::entity parent, ColliderType type);

    private:
        static entt::entity AttachCharacterCollider(entt::registry& reg, entt::entity parent, ColliderType type);
    };
}

#endif //MANGO_PREFAB_H
