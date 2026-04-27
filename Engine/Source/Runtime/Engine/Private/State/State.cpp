#include "State/State.h"

namespace tomato {
    void State::SetPlayerInput(uint32_t tick, InputRecord input, int playerID) {
        playerInputs_[playerID].SetData(tick, input);
    }
}