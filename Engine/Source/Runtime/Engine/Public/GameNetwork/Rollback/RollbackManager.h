#ifndef MANGO_ROLLBACKMANAGER_H
#define MANGO_ROLLBACKMANAGER_H

#include <vector>
#include <memory>
#include <entt/fwd.hpp>
#include "GameNetwork/Rollback/RollbackFwd.h"
#include "ECS/SystemFwd.h"

namespace tomato {
    class RollbackManager {
    public:
        RollbackManager();
        ~RollbackManager();

        template<typename Component>
        void Register() {
            timelines_.emplace_back(std::make_unique<Component>());
        }

        void Rollback(entt::registry& reg, uint32_t tick);
        void Capture(SimContext& ctx);

    private:
        std::vector<std::unique_ptr<ComponentTimelineBase>> timelines_;
    };
}

#endif //MANGO_ROLLBACKMANAGER_H