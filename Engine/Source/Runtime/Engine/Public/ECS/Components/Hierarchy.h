#ifndef MANGO_HIERARCHY_H
#define MANGO_HIERARCHY_H

#include <entt/entt.hpp>
#include <vector>

namespace tomato {
    struct HierarchyComponent {
        entt::entity parent{ entt::null };
        std::vector<entt::entity> children;
    };
}

#endif //MANGO_HIERARCHY_H
