#ifndef MANGO_ROLLBACKMANAGER_H
#define MANGO_ROLLBACKMANAGER_H

#include <vector>
#include <memory>
#include <unordered_set>
#include <typeindex>
#include <entt/fwd.hpp>
#include "GameNetwork/Rollback/ComponentTimeline.h"
#include "ECS/SystemFwd.h"
#include "ECS/EntityCompFwd.h"

namespace tomato {
    class RollbackManager {
    public:
        RollbackManager();
        ~RollbackManager();

        template<typename Component>
        requires (!std::same_as<Component, LifetimeComponent>)
        void Register() {
            if (registered_.contains(typeid(Component)))
                return;

            timelines_.emplace_back(std::make_unique<ComponentTimeline<Component>>());
            registered_.insert(typeid(Component));
        }

        void Rollback(entt::registry& reg, uint32_t tick);
        void Capture(SimContext& ctx);

    private:
        std::vector<std::unique_ptr<ComponentTimelineBase>> timelines_;

        std::unordered_set<std::type_index> registered_;
    };
}

#endif //MANGO_ROLLBACKMANAGER_H