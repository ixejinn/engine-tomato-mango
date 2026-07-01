#ifndef MANGO_COLLISIONEVENT_H
#define MANGO_COLLISIONEVENT_H

#include <entt/entt.hpp>
#include "Collision/CollisionInfo.h"

namespace tomato
{
    struct CollisionEvent
    {
        entt::entity e1, e2;
        CollisionInfo info;
    };

    struct PenetrationEvent
    {
        entt::entity e1, e2;
        entt::registry* reg;

        CollisionInfo info;
    };

    struct CollisionEnterEvent
    {
        entt::entity e1, e2;
        entt::registry* reg;

        CollisionInfo info;
    };

    struct CollisionStayEvent
    {
        entt::entity e1, e2;
        entt::registry* reg;

        CollisionInfo info;
    };

    struct CollisionExitEvent
    {
        entt::entity e1, e2;
        entt::registry* reg;
    };

    struct TriggerEnterEvent
    {
        entt::entity e1, e2;
        entt::registry* reg;
    };

    struct TriggerStayEvent
    {
        entt::entity e1, e2;
        entt::registry* reg;
    };

    struct TriggerExitEvent
    {
        entt::entity e1, e2;
        entt::registry* reg;
    };
}

#endif //MANGO_COLLISIONEVENT_H