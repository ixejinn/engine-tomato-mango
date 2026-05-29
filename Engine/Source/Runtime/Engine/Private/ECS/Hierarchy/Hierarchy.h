#ifndef MANGO_HIERARCHY_FUNCTIONS_H
#define MANGO_HIERARCHY_FUNCTIONS_H

#include <entt/fwd.hpp>

namespace tomato {
    void SetParent(entt::registry& reg, entt::entity child, entt::entity parent);
}

#endif //MANGO_HIERARCHY_FUNCTIONS_H
