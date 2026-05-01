#ifndef MANGO_COLLISIONEVENT_H
#define MANGO_COLLISIONEVENT_H

#include <entt/entt.hpp>

namespace tomato {
    struct CollisionEnterEvent {
        entt::entity e1, e2;
        entt::registry* reg;

        glm::vec3 normal;
        float depth;
    };

    struct CollisionStayEvent {
        entt::entity e1, e2;
        entt::registry* reg;
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