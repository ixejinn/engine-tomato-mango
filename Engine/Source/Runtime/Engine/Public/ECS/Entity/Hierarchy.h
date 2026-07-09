#ifndef MANGO_ENTITY_HIERARCHY_H
#define MANGO_ENTITY_HIERARCHY_H

#include <entt/fwd.hpp>

namespace tomato {
    entt::entity GetRootEntity(entt::registry& reg, entt::entity cur);
    entt::entity GetRootEntity(entt::registry* reg, entt::entity cur);

    void SetHierarchy(entt::registry& reg, entt::entity parent, entt::entity child);

    void DestroyHierarchyEntity(entt::registry& reg, entt::entity parent);

    bool IsDescendant(entt::registry& reg, entt::entity newParent, entt::entity child);
}

#endif //MANGO_ENTITY_HIERARCHY_H