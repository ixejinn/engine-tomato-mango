#include "ECS/Components/Hierarchy.h"
#include "ECS/Entity/Hierarchy.h"
#include "ECS/Entity/Entity.h"

namespace tomato {
    entt::entity GetRootEntity(entt::registry& registry, entt::entity cur)
    {
        if (auto* hierarchy = registry.try_get<HierarchyComponent>(cur))
        {
            if (hierarchy->root == entt::null)
                hierarchy->root = cur;
            return hierarchy->root;
        }
        else
            return cur;
    }

    entt::entity GetRootEntity(entt::registry* registry, entt::entity cur)
    {
        if (auto* hierarchy = registry->try_get<HierarchyComponent>(cur))
        {
            if (hierarchy->root == entt::null)
                hierarchy->root = cur;
            return hierarchy->root;
        }
        else
            return cur;
    }

    void SetHierarchy(entt::registry& registry, entt::entity parent, entt::entity child)
    {
        if (IsDescendant(registry, parent, child))
            return;

        auto& cHierarchy = registry.get_or_emplace<HierarchyComponent>(child);

        // child의 parent가 있었다면 parent의 HierarchyComponent에서 child 지우기
        if (cHierarchy.parent != entt::null)
        {
            auto& prePHierarchy = registry.get<HierarchyComponent>(cHierarchy.parent);
            std::erase(prePHierarchy.childrenUUID, GetUUID(registry, child));
            std::erase(prePHierarchy.children, child);
        }
        cHierarchy.parentUUID = GetUUID(registry, parent);
        cHierarchy.parent = parent;

        // parent 설정
        if (parent != entt::null)
        {
            auto& pHierarchy = registry.get_or_emplace<HierarchyComponent>(parent);

            pHierarchy.childrenUUID.push_back(GetUUID(registry, child));
            pHierarchy.children.push_back(child);

            if (pHierarchy.root == entt::null)
                pHierarchy.root = parent;
            cHierarchy.root = pHierarchy.root;

            registry.remove<RootEntityTag>(child);
        }
        // child를 root로 만듦
        else
        {
            registry.get_or_emplace<RootEntityTag>(child);

            cHierarchy.parentUUID = 0;
            cHierarchy.root = child;
            cHierarchy.parent = entt::null;
        }
    }

    bool IsDescendant(entt::registry& registry, entt::entity descendant, entt::entity ancestor)
    {
        entt::entity current = descendant;

        while (current != entt::null)
        {
            if (current == ancestor)
                return true;

            auto* hierarchy = registry.try_get<HierarchyComponent>(current);
            if (!hierarchy)
                break;
            
            current = hierarchy->parent;
        }

        return false;
    }

    void DestroyHierarchySubtree(entt::registry& registry, entt::entity entity)
    {
        auto* hierarchy = registry.try_get<HierarchyComponent>(entity);
        if (!hierarchy)
        {
            if(registry.valid(entity))
                registry.destroy(entity);

            return;
        }

        // if this entity has a parent, remove it from parent's children list
        if (hierarchy->parent != entt::null)
        {
            auto& pHierarchy = registry.get<HierarchyComponent>(hierarchy->parent);
            std::erase(pHierarchy.children, entity);
        }

        // destroy hierarchy subtree
        for (entt::entity child : hierarchy->children)
            DestroyHierarchySubtree(registry, child);

        registry.destroy(entity);
    }
}