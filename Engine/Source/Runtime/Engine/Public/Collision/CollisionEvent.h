#ifndef MANGO_COLLISIONEVENT_H
#define MANGO_COLLISIONEVENT_H

#include <glm/vec3.hpp>
#include <entt/entt.hpp>
#include "Collision/CollisionResult.h"

namespace tomato {
    struct PenetrationEvent {
        entt::entity e1, e2;
        entt::registry* reg;

        CollisionResult info;
    };

    struct CollisionEnterEvent {
        entt::entity e1, e2;
        entt::registry* reg;

        CollisionResult info;
    };

    struct CollisionStayEvent {
        entt::entity e1, e2;
        entt::registry* reg;

        CollisionResult info;
    };

    struct CollisionExitEvent {
        entt::entity e1, e2;
        entt::registry* reg;
    };

    struct TriggerEnterEvent {
        entt::entity e1, e2;
        entt::registry* reg;
    };

    struct TriggerStayEvent {
        entt::entity e1, e2;
        entt::registry* reg;
    };

    struct TriggerExitEvent {
        entt::entity e1, e2;
        entt::registry* reg;
    };
}

#endif //MANGO_COLLISIONEVENT_H