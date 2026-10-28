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
        entt::entity root{entt::null};
        entt::entity parent{entt::null};
        std::vector<entt::entity> children;
    };
}

#endif //MANGO_HIERARCHY_H
