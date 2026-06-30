#include "State/State.h"
#include "ECS/SystemUpdateContexts.h"

namespace tomato
{
    State::State(Engine& engine) : engine_(engine)
    {
        registry_.ctx().emplace<RenderContext>();
        registry_.ctx().emplace<CollisionContext>();
    }

    void State::SetPlayerInput(uint32_t tick, InputRecord input, int playerID)
    {
        playerInputs_[playerID].SetData(tick, input);
    }
}