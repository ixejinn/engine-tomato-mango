#ifndef MANGO_COLLISION_H
#define MANGO_COLLISION_H

#include <glm/vec3.hpp>
#include "Collision/CollisionConstants.h"

namespace tomato
{
    struct ColliderComponent
    {
        ColliderComponent() = default;

        ColliderComponent(bool trigger) : trigger(trigger) {}

        ColliderComponent(
            const ColliderType t,
            const bool trigger = false,
            const CollisionLayer l = CollisionLayer::Default)
                : layer(l), type(t), trigger(trigger) {}

        glm::vec3 min{}, max{}; // AABB for broad-phase collision detection

        CollisionLayer layer{CollisionLayer::Default};
        ColliderType type{ColliderType::Cube};

        bool trigger{false};
        bool aabbDirty{true};
    };
}

#endif //MANGO_COLLISION_H