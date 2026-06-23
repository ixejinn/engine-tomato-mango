#include <entt/entt.hpp>
#include "GameNetwork/Rollback/RollbackManager.h"
#include "GameNetwork/Rollback/ComponentTimeline.h"
#include "ECS/Components/Lifetime.h"
#include "ECS/SystemUpdateContexts.h"

namespace tomato {
    RollbackManager::RollbackManager() {
        timelines_.emplace_back(std::make_unique<ComponentTimeline<LifetimeComponent>>());
    }

    RollbackManager::~RollbackManager() = default;

    void RollbackManager::Rollback(entt::registry& reg, uint32_t tick) {
        for (auto& timeline : timelines_)
            timeline->Restore(reg, tick);
    }

    void RollbackManager::Capture(SimContext& ctx) {
        for (auto& timeline : timelines_)
            timeline->Record(ctx.registry, ctx.tick);
    }
}