#ifndef MANGO_HIERARCHY_H
#define MANGO_HIERARCHY_H

#include <entt/entt.hpp>
#include <vector>

namespace tomato {
    struct RootEntityTag {};

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

    inline entt::entity GetRootEntity(entt::registry* reg, entt::entity cur) {
        entt::entity root = cur;

        auto* hierarchy = reg->try_get<HierarchyComponent>(root);
        while (hierarchy && hierarchy->parent != entt::null) {
            root = hierarchy->parent;
            hierarchy = reg->try_get<HierarchyComponent>(root);
        }

        return root;
    }

    inline void SetHierarchy(entt::registry& reg, entt::entity parent, entt::entity child) {
        auto* cHierarchy = reg.try_get<HierarchyComponent>(child);
        if (!cHierarchy)
            cHierarchy = &(reg.emplace<HierarchyComponent>(child));

        if (cHierarchy->parent != entt::null) {
            auto& oldPHierarchy = reg.get<HierarchyComponent>(cHierarchy->parent);
            auto& siblings = oldPHierarchy.children;
            // siblings.erase(std::remove(siblings.begin(), siblings.end(), child), siblings.end());
            std::erase(siblings, child);
        }
        cHierarchy->parent = parent;

        if (parent != entt::null) {
            auto* newPHierarchy = reg.try_get<HierarchyComponent>(parent);
            if (!newPHierarchy)
                newPHierarchy = &(reg.emplace<HierarchyComponent>(parent));

            newPHierarchy->children.push_back(child);

            auto root = GetRootEntity(reg, parent);
            if (!reg.all_of<RootEntityTag>(root))
                reg.emplace<RootEntityTag>(root);

            if (reg.all_of<RootEntityTag>(child))
                reg.remove<RootEntityTag>(child);
        }
        else {
            if (!reg.all_of<RootEntityTag>(child))
                reg.emplace<RootEntityTag>(child);
        }
    }
}

#endif //MANGO_HIERARCHY_H
