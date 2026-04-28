#include "Tick/TickClock.h"

namespace tomato {
    int TickClock::GetSimulateNum() {
        std::chrono::steady_clock::time_point cur = std::chrono::steady_clock::now();
        adder_ += std::chrono::duration<float, std::milli>(cur - start_);

        start_ = cur;

        const int simulateNum = std::min(static_cast<int>(adder_ / dt_), MAX_SIMULATION_NUM);
        adder_ -= dt_ * simulateNum;

        return simulateNum;
    }

    void TickClock::ResetTick() {
        tick_ = 0;
        start_ = std::chrono::steady_clock::now();
        adder_ = std::chrono::milliseconds::zero();
    }
}