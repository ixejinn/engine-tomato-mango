#ifndef MANGO_ONCOLLISION_H
#define MANGO_ONCOLLISION_H

#include "Collision/CollisionEvent.h"

namespace tomato
{
    struct OnPenetrationComponent
    {
        entt::entity e1, e2;
        CollisionInfo info;
    };

    struct OnCollisionComponent
    {
        CollisionEventType type;
        entt::entity e1, e2;
        CollisionInfo info;
    };

    struct OnTriggerComponent
    {
        CollisionEventType type;
        entt::entity e1, e2;
    };
}

#endif //MANGO_ONCOLLISION_H
