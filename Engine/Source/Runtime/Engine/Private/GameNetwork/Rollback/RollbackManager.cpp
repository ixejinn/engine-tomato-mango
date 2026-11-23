#include <entt/entt.hpp>
#include "GameNetwork/Rollback/RollbackManager.h"
#include "GameNetwork/Rollback/SnapshotTimeline.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Velocity.h"
#include "ECS/Components/Movement.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"

namespace tomato {
    RollbackManager::RollbackManager() {
        timelines_.emplace_back(std::make_unique<SnapshotTimeline<CollisionContext>>());

        timelines_.emplace_back(std::make_unique<SnapshotTimeline<TransformComponent>>());
        registered_.insert(typeid(TransformComponent));

        timelines_.emplace_back(std::make_unique<SnapshotTimeline<VelocityComponent>>());
        registered_.insert(typeid(VelocityComponent));

        timelines_.emplace_back(std::make_unique<SnapshotTimeline<MovementComponent>>());
        registered_.insert(typeid(MovementComponent));
    }

    RollbackManager::~RollbackManager() = default;

    void RollbackManager::Rollback(const SimContext& ctx) {
        for (auto& timeline : timelines_)
            timeline->Rollback(ctx.state->GetRegistry(), ctx.tick);
    }

    void RollbackManager::Capture(const SimContext& ctx) {
        for (auto& timeline : timelines_)
            timeline->Capture(ctx.state->GetRegistry(), ctx.tick);
    }
}