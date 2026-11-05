#include "Simulation/Tick/TickClock.h"

namespace tomato
{
    int TickClock::GetSimulateNum(TickPolicy policy)
    {
        const std::chrono::steady_clock::time_point cur = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(cur - start_);
        start_ = cur;

        bool discard = (policy == TickPolicy::Discard);
        constexpr std::chrono::milliseconds TICK_THRESHOLD{250};
        if (discard && elapsed > TICK_THRESHOLD)
            elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(DT_);

        adder_ += elapsed;

        const int pending = static_cast<int>(std::chrono::duration_cast<Tick>(adder_).count());
        const int simulateNum = std::min(pending, MAX_SIMULATION_NUM_PER_FRAME);

        adder_ -= std::chrono::duration_cast<std::chrono::nanoseconds>(Tick{simulateNum});

        if (discard && pending > MAX_SIMULATION_NUM_PER_FRAME)
            adder_ = std::chrono::nanoseconds::zero();

        return simulateNum;
    }

    void TickClock::ResetTick()
    {
        tick_ = Tick::zero();
        start_ = std::chrono::steady_clock::now();
        adder_ = std::chrono::nanoseconds::zero();
    }
}