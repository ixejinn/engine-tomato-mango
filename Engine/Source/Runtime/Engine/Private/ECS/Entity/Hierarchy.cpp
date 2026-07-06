#include "ECS/Components/Hierarchy.h"
#include "ECS/Entity/Hierarchy.h"

namespace tomato {
    entt::entity GetRootEntity(entt::registry& reg, entt::entity cur)
    {
        entt::entity root = cur;

        auto* hierarchy = reg.try_get<HierarchyComponent>(root);
        while (hierarchy && hierarchy->parent != entt::null) {
            root = hierarchy->parent;
            hierarchy = reg.try_get<HierarchyComponent>(root);
        }

        return root;
    }

    entt::entity GetRootEntity(entt::registry* reg, entt::entity cur)
    {
        entt::entity root = cur;

        auto* hierarchy = reg->try_get<HierarchyComponent>(root);
        while (hierarchy && hierarchy->parent != entt::null) {
            root = hierarchy->parent;
            hierarchy = reg->try_get<HierarchyComponent>(root);
        }

        return root;
    }

    void SetHierarchy(entt::registry& reg, entt::entity parent, entt::entity child) {
        auto* cHierarchy = reg.try_get<HierarchyComponent>(child);
        if (!cHierarchy)
            cHierarchy = &(reg.emplace<HierarchyComponent>(child));

        if (cHierarchy->parentID != 0) {
            auto& oldPHierarchy = reg.get<HierarchyComponent>(GetEntityByUUID(reg, cHierarchy->parentID));
            auto& siblingsID = oldPHierarchy.childrenID;
            auto& siblings = oldPHierarchy.children;
            // siblings.erase(std::remove(siblings.begin(), siblings.end(), child), siblings.end());
            std::erase(siblingsID, GetUUID(reg, child));
            std::erase(siblings, child);
        }
        cHierarchy->parentID = GetUUID(reg, parent);
        cHierarchy->parent = parent;

        if (parent != entt::null) {
            auto* newPHierarchy = reg.try_get<HierarchyComponent>(parent);
            if (!newPHierarchy)
                newPHierarchy = &(reg.emplace<HierarchyComponent>(parent));

            newPHierarchy->childrenID.push_back(GetUUID(reg, child));
            newPHierarchy->children.push_back(child);

            auto root = GetRootEntity(reg, parent);
            if (!reg.all_of<RootEntityTag>(root))
            {
                reg.emplace<RootEntityTag>(root);
                auto& rootHierarchy = reg.get<HierarchyComponent>(root);
                rootHierarchy.parentID = 0;
            }

            if (reg.all_of<RootEntityTag>(child))
                reg.remove<RootEntityTag>(child);
        }
        else {
            if (!reg.all_of<RootEntityTag>(child))
                reg.emplace<RootEntityTag>(child);
        }
    }

    void DestroyHierarchyEntity(entt::registry& reg, entt::entity parent)
    {
        auto* hierarchy = reg.try_get<HierarchyComponent>(parent);
        if (!hierarchy)
        {
            reg.destroy(parent);
            return;
        }

        // if this entity has a parent,
        // remove it from parent's children list
        if (hierarchy && hierarchy->parent != entt::null)
        {
            auto& pHierarchy = reg.get<HierarchyComponent>(hierarchy->parent);
            std::erase(pHierarchy.children, parent);
        }

        for (entt::entity child : hierarchy->children)
            DestroyHierarchyEntity(reg, child);

        reg.destroy(parent);
    }
}