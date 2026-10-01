#include "ECS/Systems/GarbageEntityCollectionSystem.h"
#include "ECS/Components/PendingDestroy.h"
#include "ECS/Entity/Entity.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "GameNetwork/Rollback/RollbackConfig.h"

namespace tomato
{
    void GarbageEntityCollectionSystem::Update(SimContext &simCtx)
    {
        auto& registry = simCtx.state->GetRegistry();
        auto view = registry.view<PendingDestroyComponent>();
        for (auto [e, destroy] : view.each())
        {
            if (simCtx.tick - destroy.destroyed > ROLLBACK_WINDOW)
                registry.destroy(e);
        }
    }
}
