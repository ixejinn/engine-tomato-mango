#ifndef MANGO_WAVEEVENT_H
#define MANGO_WAVEEVENT_H

#include <entt/fwd.hpp>

struct WaveCollisionEvent
{
    entt::entity e;
    entt::registry* registry{nullptr};
};

#endif //MANGO_WAVEEVENT_H
