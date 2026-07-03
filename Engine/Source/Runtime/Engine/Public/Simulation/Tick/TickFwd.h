#ifndef MANGO_TICKFWD_H
#define MANGO_TICKFWD_H

#include <chrono>
#include "Simulation/SimulationConfig.h"

namespace tomato {
    using Tick = std::chrono::duration<int64_t, std::ratio<1, FRAME_PER_SECOND>>;

    class TickClock;
}

#endif //MANGO_TICKFWD_H