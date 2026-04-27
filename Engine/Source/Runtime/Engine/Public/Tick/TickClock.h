#ifndef MANGO_TICKCLOCK_H
#define MANGO_TICKCLOCK_H

#include <chrono>
#include "SimulationConfig.h"

namespace tomato {
    class TickClock {
    public:
        uint32_t GetTick() const { return tick_; }

        int GetSimulateNum();

        void AddTick() { ++tick_; }
        void ResetTick();

    private:
        static constexpr int MAX_SIMULATION_NUM = 3;
        static constexpr std::chrono::duration<float, std::milli> dt_{1000.f / FRAME_PER_SECOND};

        uint32_t tick_{0};

        std::chrono::duration<float, std::milli> adder_{0};
        std::chrono::steady_clock::time_point start_{};
    };
}

#endif //MANGO_TICKCLOCK_H