#include "ECS/Systems/CharacterScaleSystem.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "ECS/Components/Character.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/TransformDirty.h"
#include "Collision/CollisionConstants.h"
#include "ECS/Entity/Hierarchy.h"
#include "Utils/Bitmask/BitmaskOperators.h"
#include "GameObject/Character/CharacterConfig.h"

namespace tomato
{
    void CharacterScaleSystem::Update(SimContext& simCtx)
    {
        auto& registry = simCtx.state->GetRegistry();

        auto view = registry.view<GroundTriggerTag, TransformComponent>();
        for (auto [e, trf] : view.each())
        {
            auto& trfRoot = registry.get<TransformComponent>(GetRootEntity(registry, e));
            if (HasFlag(trfRoot.dirty, Transform::Dirty::Scale))
            {
                const float deltaPosY = (1 - Character::GROUND_TRIGGER_SCALE) * 0.5
                                      + COLLISION_SKIN * Character::GROUND_TRIGGER_EXTENSION_RATIO / trfRoot.scale.y;
                trf.position.y = -deltaPosY;

                trfRoot.dirty &= ~Transform::Dirty::Scale;
                trf.dirty |= Transform::Dirty::Local;
            }
        }
    }
}
