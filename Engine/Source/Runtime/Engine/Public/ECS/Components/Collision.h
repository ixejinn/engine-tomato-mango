#ifndef MANGO_COLLISION_H
#define MANGO_COLLISION_H

#include "Math/AABB.h"
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

        CollisionLayer layer{CollisionLayer::Default};

        AABB aabb;
        ColliderType type{ColliderType::Cube};

        bool trigger{false};
        bool aabbDirty{true};
    };
}

#endif //MANGO_COLLISION_H