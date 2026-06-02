#ifndef MANGO_HIERARCHY_H
#define MANGO_HIERARCHY_H

#include <entt/entt.hpp>
#include <vector>

namespace tomato {
    struct HierarchyComponent {
        entt::entity parent{ entt::null };
        std::vector<entt::entity> children;
    };

    inline entt::entity GetRootEntity(entt::registry& reg, entt::entity cur) {
        entt::entity root = cur;

        auto* hierarchy = reg.try_get<HierarchyComponent>(root);
        while (hierarchy && hierarchy->parent != entt::null) {
            root = hierarchy->parent;
            hierarchy = reg.try_get<HierarchyComponent>(root);
        }

        return root;
    }
}

#endif //MANGO_HIERARCHY_H
