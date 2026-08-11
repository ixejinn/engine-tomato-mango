#include "State/State.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "Particle/ParticleEmitterPool.h"
#include "Utils/PassKey.h"

namespace tomato
{
    State::State(Engine& engine) : engine_(engine)
    {
        registry_.ctx().emplace<RenderContext>();
        registry_.ctx().emplace<CollisionContext>();
        registry_.ctx().emplace<ParticleEmitterPool>(PassKey<State>(), registry_);
    }

    void State::SetPlayerInput(uint32_t tick, InputRecord input, int playerID)
    {
        playerInputs_[playerID].SetData(tick, input);
    }
}