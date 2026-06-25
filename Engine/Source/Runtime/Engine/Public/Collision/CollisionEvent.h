#ifndef MANGO_COLLISIONEVENT_H
#define MANGO_COLLISIONEVENT_H

#include <glm/vec3.hpp>
#include <entt/entt.hpp>
#include "Collision/CollisionInfo.h"

namespace tomato {
    struct PenetrationEvent {
        entt::entity e1, e2;
        entt::registry* reg;

        CollisionInfo info;
    };

    struct CollisionEnterEvent {
        entt::entity e1, e2;
        entt::registry* reg;

        CollisionInfo info;
        uint32_t tick;
    };

    struct CollisionStayEvent {
        entt::entity e1, e2;
        entt::registry* reg;

        CollisionInfo info;
    };

    struct CollisionExitEvent {
        entt::entity e1, e2;
        entt::registry* reg;
        uint32_t tick;
    };

    struct TriggerEnterEvent {
        entt::entity e1, e2;
        entt::registry* reg;
        uint32_t tick;
    };

    struct TriggerStayEvent {
        entt::entity e1, e2;
        entt::registry* reg;
    };

    struct TriggerExitEvent {
        entt::entity e1, e2;
        entt::registry* reg;
        uint32_t tick;
    };
}

#endif //MANGO_COLLISIONEVENT_H