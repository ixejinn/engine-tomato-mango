#ifndef MANGO_STATEFWD_H
#define MANGO_STATEFWD_H

#include <array>
#include "Containers/Timeline.h"
#include "Input/InputRecord.h"
#include "GameplayConfig.h"

namespace tomato {
    class State;

    using PlayerInputTimelines = std::array<Timeline<InputRecord>, MAX_PLAYER_NUM>;
}

#endif //MANGO_STATEFWD_H