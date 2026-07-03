#include "Simulation/Tick/TickClock.h"

namespace tomato {
    int TickClock::GetSimulateNum() {
        std::chrono::steady_clock::time_point cur = std::chrono::steady_clock::now();
        adder_ += std::chrono::duration_cast<std::chrono::nanoseconds>(cur - start_);

        start_ = cur;

        auto ticksToSimulate = std::chrono::duration_cast<Tick>(adder_);
        const int simulateNum = std::min(
            static_cast<int>(ticksToSimulate.count()),
            MAX_SIMULATION_NUM_PER_FRAME);
        adder_ -= std::chrono::duration_cast<std::chrono::nanoseconds>(Tick{simulateNum});

        return simulateNum;
    }

    void TickClock::ResetTick() {
        tick_ = Tick::zero();
        start_ = std::chrono::steady_clock::now();
        adder_ = std::chrono::nanoseconds::zero();
    }
}