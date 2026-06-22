#include <entt/entt.hpp>
#include "ECS/Entity/Lifetime.h"
#include "ECS/Entity/Hierarchy.h"
#include "ECS/Components/Hierarchy.h"

namespace tomato {
    void DestroyEntity(entt::registry& reg, entt::entity e) {
        if (reg.try_get<HierarchyComponent>(e))
            DestroyHierarchyEntity(reg, e);
        else
            reg.destroy(e);
    }
}