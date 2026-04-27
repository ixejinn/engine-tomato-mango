#ifndef MANGO_COLLISION_H
#define MANGO_COLLISION_H

#include <glm/vec3.hpp>
#include "Collision/CollisionConstants.h"

namespace tomato {
    struct CollisionEvent;
    struct TriggerEvent;

    struct ColliderComponent {
        ColliderComponent(const ColliderType t,
            const CollisionLayer l = CollisionLayer::Default,
            const glm::vec3 pos = glm::vec3{0.f})
                : position(pos), layer(l), type(t) {}

        glm::vec3 position;
        glm::vec3 halfExtents{0.5f, 0.5f, 0.5f};
        glm::vec3 min{}, max{}; // AABB for broad-phase collision detection

        CollisionLayer layer;
        ColliderType type;

        bool isTrigger = false;
        bool aabbDirty = true;
    };

    // Callback function
    using CollisionCallback = std::function<void(const CollisionEvent&)>;
    struct OnCollisionComponent
    {
        CollisionCallback enter;
        // exit, stay
    };

    using TriggerCallback = std::function<void(const TriggerEvent&)>;
    struct OnTriggerComponent
    {
        TriggerCallback enter;
    };
}

#endif //MANGO_COLLISION_H