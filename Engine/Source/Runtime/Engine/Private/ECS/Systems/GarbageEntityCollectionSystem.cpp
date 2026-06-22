#include "ECS/Systems/GarbageEntityCollectionSystem.h"
#include "ECS/Components/Lifetime.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Entity/Lifetime.h"
#include "ECS/SystemUpdateContexts.h"
#include "GameNetwork/Rollback/RollbackConfig.h"

namespace tomato {
    void GarbageEntityCollectionSystem::Update(SimContext &simCtx) {
        auto view = simCtx.registry.view<Lifetime>();

        for (auto [e, life] : view.each()) {
            if (life.isActive || life.destructed == std::nullopt)
                continue;

            if (simCtx.tick - life.destructed.value() > ROLLBACK_WINDOW)
                DestroyEntity(simCtx.registry, e);
        }
    }
}
