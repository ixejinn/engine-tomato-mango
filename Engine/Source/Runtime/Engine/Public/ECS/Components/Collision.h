#ifndef MANGO_COLLISION_H
#define MANGO_COLLISION_H

#include <glm/vec3.hpp>
#include <entt/fwd.hpp>
#include "Collision/CollisionConstants.h"
#include "Collision/CollisionEventFwd.h"
#include "ECS/Components/Transform.h"

namespace tomato {
    struct ColliderComponent {
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

    // Component for callback function
//    using CollisionEnterCallback = std::function<void(const CollisionEnterEvent&, entt::entity)>;
//    using CollisionStayCallback = std::function<void(const CollisionStayEvent&, entt::entity)>;
//    using CollisionExitCallback = std::function<void(const CollisionExitEvent&, entt::entity)>;
//    struct OnCollisionComponent
//    {
//        CollisionEnterCallback enter{nullptr};
//        CollisionStayCallback stay{nullptr};
//        CollisionExitCallback exit{nullptr};
//    };
//
//    using TriggerEnterCallback = std::function<void(const TriggerEnterEvent&, entt::entity)>;
//    using TriggerStayCallback = std::function<void(const TriggerStayEvent&, entt::entity)>;
//    using TriggerExitCallback = std::function<void(const TriggerExitEvent&, entt::entity)>;
//    struct OnTriggerComponent
//    {
//        TriggerEnterCallback enter{nullptr};
//        TriggerStayCallback stay{nullptr};
//        TriggerExitCallback exit{nullptr};
//    };
}

#endif //MANGO_COLLISION_H