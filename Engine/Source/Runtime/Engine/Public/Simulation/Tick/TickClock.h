#ifndef MANGO_TICKCLOCK_H
#define MANGO_TICKCLOCK_H

#include <chrono>
#include <ratio>
#include "Simulation/SimulationConfig.h"

namespace tomato {
    using Tick = std::chrono::duration<int64_t, std::ratio<1, FRAME_PER_SECOND>>;

    class TickClock {
    public:
        [[nodiscard]] int64_t GetTick() const { return tick_.count(); }

        int GetSimulateNum();

        void AddTick() { ++tick_; }
        void ResetTick();

    private:
        static constexpr Tick DT_{1};
        Tick tick_{0};

        std::chrono::nanoseconds adder_{0};
        std::chrono::steady_clock::time_point start_{};
    };
}

#endif //MANGO_TICKCLOCK_H