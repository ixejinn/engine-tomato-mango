#include "State/State.h"
#include "ECS/SystemUpdateContexts.h"
#include "Utils/PassKey.h"

namespace tomato
{
    State::State(Engine& engine) : engine_(engine), particlePool_(PassKey<State>(), registry_)
    {
        registry_.ctx().emplace<RenderContext>();
        registry_.ctx().emplace<CollisionContext>();
    }

    void State::SetPlayerInput(uint32_t tick, InputRecord input, int playerID)
    {
        playerInputs_[playerID].SetData(tick, input);
    }
}