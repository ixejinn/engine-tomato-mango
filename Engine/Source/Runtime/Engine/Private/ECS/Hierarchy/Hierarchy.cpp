#include <entt/entt.hpp>
#include "ECS/Hierarchy/Hierarchy.h"
#include "ECS/Components/Hierarchy.h"
#include "Prefab/EntityUtils.h"

namespace tomato {
    void SetParent(entt::registry& reg, entt::entity child, entt::entity parent) {
        auto& cHierarchy = reg.get<HierarchyComponent>(child);
        if (cHierarchy.parentID != 0) {
            auto& oldPHierarchy = reg.get<HierarchyComponent>(GetEntityByUUID(reg, cHierarchy.parentID));
            auto& siblingsID = oldPHierarchy.childrenID;
            auto& siblings = oldPHierarchy.children;

            std::erase(siblingsID, GetUUID(reg, child));
            std::erase(siblings, child);
        }

        cHierarchy.parentID = GetUUID(reg, parent);
        cHierarchy.parent = parent;

        if (parent != entt::null) {
            auto& newPHierarchy = reg.get<HierarchyComponent>(parent);
            newPHierarchy.childrenID.push_back(GetUUID(reg, child));
            newPHierarchy.children.push_back(child);
        }
    }
}