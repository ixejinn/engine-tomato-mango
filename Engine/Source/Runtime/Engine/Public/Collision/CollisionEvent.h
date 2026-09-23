#ifndef MANGO_COLLISIONEVENT_H
#define MANGO_COLLISIONEVENT_H

#include <entt/entt.hpp>
#include "Collision/ContactData.h"

namespace tomato
{
    struct TriggerEvent
    {
        entt::entity a, b;
        entt::registry* reg;
    };

    struct ContactEvent : TriggerEvent
    {
        ContactData data;
    };

    struct CollisionEnterEvent : ContactEvent {};
    struct CollisionStayEvent  : ContactEvent {};
    struct CollisionExitEvent  : TriggerEvent {};

    struct TriggerEnterEvent : TriggerEvent {};
    struct TriggerStayEvent  : TriggerEvent {};
    struct TriggerExitEvent  : TriggerEvent {};
}

#endif //MANGO_COLLISIONEVENT_H