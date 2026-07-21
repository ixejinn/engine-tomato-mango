#include <entt/entt.hpp>
#include "GameNetwork/Rollback/RollbackManager.h"
#include "GameNetwork/Rollback/ComponentTimeline.h"
#include "ECS/Components/Lifetime.h"
#include "ECS/Components/Transform.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"

namespace tomato {
    RollbackManager::RollbackManager() {
        timelines_.emplace_back(std::make_unique<ComponentTimeline<CollisionPair>>());

        timelines_.emplace_back(std::make_unique<ComponentTimeline<LifetimeComponent>>());
        registered_.insert(typeid(LifetimeComponent));

        timelines_.emplace_back(std::make_unique<ComponentTimeline<TransformComponent>>());
        registered_.insert(typeid(TransformComponent));
    }

    RollbackManager::~RollbackManager() = default;

    void RollbackManager::Rollback(const SimContext& ctx) {
        for (auto& timeline : timelines_)
            timeline->Restore(ctx.state->GetRegistry(), ctx.tick);
    }

    void RollbackManager::Capture(const SimContext& ctx) {
        for (auto& timeline : timelines_)
            timeline->Record(ctx.state->GetRegistry(), ctx.tick);
    }
}