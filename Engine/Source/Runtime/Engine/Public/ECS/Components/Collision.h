#ifndef MANGO_COLLISION_H
#define MANGO_COLLISION_H

#include <glm/vec3.hpp>
#include <entt/fwd.hpp>
#include "Collision/CollisionConstants.h"
#include "Collision/CollisionEventFwd.h"
#include "ECS/Components/Transform.h"

namespace tomato
{
    struct ColliderComponent
    {
        ColliderComponent(
            const ColliderType t,
            const bool trigger = false,
            const CollisionLayer l = CollisionLayer::Default)
                : layer(l), type(t), isTrigger(trigger) {}

        glm::vec3 min{}, max{}; // AABB for broad-phase collision detection

        CollisionLayer layer;
        ColliderType type;

        bool isTrigger;
        bool aabbDirty = true;
    };
}

#endif //MANGO_COLLISION_H