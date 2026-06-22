#ifndef MANGO_ENTITY_LIFETIME_H
#define MANGO_ENTITY_LIFETIME_H

#include <entt/fwd.hpp>

namespace tomato {
    void DestroyEntity(entt::registry& reg, entt::entity e);
}

#endif //MANGO_ENTITY_LIFETIME_H