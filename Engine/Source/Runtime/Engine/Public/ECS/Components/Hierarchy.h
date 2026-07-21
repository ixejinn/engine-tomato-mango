#ifndef MANGO_HIERARCHY_H
#define MANGO_HIERARCHY_H

#include <entt/entt.hpp>
#include <vector>

#include "ECS/Entity/Entity.h"

namespace tomato
{
    struct RootEntityTag {};

    struct HierarchyComponent
    {
        UUID parentID{0};
        std::vector<UUID> childrenID;

        //cache
        entt::entity parent{entt::null};
        std::vector<entt::entity> children;
    };
}

#endif //MANGO_HIERARCHY_H
