#include <entt/entt.hpp>
#include "ECS/Hierarchy/Hierarchy.h"
#include "ECS/Components/Hierarchy.h"

namespace tomato {
    void SetParent(entt::registry& reg, entt::entity child, entt::entity parent) {
        auto& cHierarchy = reg.get<HierarchyComponent>(child);
        if (cHierarchy.parent != entt::null) {
            auto& oldPHierarchy = reg.get<HierarchyComponent>(cHierarchy.parent);
            auto& siblings = oldPHierarchy.children;

//            siblings.erase(std::remove(siblings.begin(), siblings.end(), child), siblings.end());
            std::erase(siblings, child);
        }

        cHierarchy.parent = parent;
        if (parent != entt::null) {
            auto& newPHierarchy = reg.get<HierarchyComponent>(parent);
            newPHierarchy.children.push_back(child);
        }
    }
}