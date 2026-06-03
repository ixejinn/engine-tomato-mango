#ifndef MANGO_COLLIDER_H
#define MANGO_COLLIDER_H

#include "Objects/Object.h"

namespace tomato {
    class Collider : public Object {
    public:
        Collider(entt::registry& reg, entt::entity parent);

    private:

    };
}

#endif //MANGO_COLLIDER_H