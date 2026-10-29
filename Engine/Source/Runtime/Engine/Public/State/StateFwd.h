#ifndef MANGO_STATEFWD_H
#define MANGO_STATEFWD_H

#include <array>
#include "Containers/Timeline.h"
#include "Input/IntentState.h"
#include "Engine/GameplayConfig.h"

namespace tomato
{
    class State;

    using PlayerInputTimelines = std::array<Timeline<IntentState>, MAX_PLAYER_NUM>;

    namespace Event
    {
        struct ChangeStateRequest;
    }
}

#endif //MANGO_STATEFWD_H