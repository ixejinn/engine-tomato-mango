#ifndef MANGO_COLLISIONEVENT_H
#define MANGO_COLLISIONEVENT_H

#include <entt/fwd.hpp>

namespace tomato {
    struct CollisionEvent {
        entt::entity e1, e2;
    };

    struct TriggerEvent {
        entt::entity e1, e2;
    };
}

#endif //MANGO_COLLISIONEVENT_H