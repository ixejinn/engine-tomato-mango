#ifndef MANGO_ENTITY_HIERARCHY_H
#define MANGO_ENTITY_HIERARCHY_H

#include <entt/fwd.hpp>

namespace tomato {
    entt::entity GetRootEntity(entt::registry& registry, entt::entity cur);
    entt::entity GetRootEntity(entt::registry* registry, entt::entity cur);

    void SetHierarchy(entt::registry& registry, entt::entity parent, entt::entity child);
    bool IsDescendant(entt::registry& registry, entt::entity descendant, entt::entity ancestor);

    void DestroyHierarchySubtree(entt::registry& registry, entt::entity entity);
}

#endif //MANGO_ENTITY_HIERARCHY_H