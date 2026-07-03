#ifndef MANGO_TICKTIMER_H
#define MANGO_TICKTIMER_H

#include <string>
#include "Simulation/Tick/TickFwd.h"
#include "Clock/TimerState.h"

namespace tomato
{
    class TickTimer
    {
    public:
        TickTimer() = default;
        TickTimer(std::chrono::hours hr, std::chrono::minutes min, std::chrono::seconds sec);

        [[nodiscard]] TimerState GetState() const { return state_; }

        bool IsTimeUp(int64_t curTick);
        std::string GetString(int64_t curTick, bool padMinutes = false, bool showHours = false);

        void Start(int64_t curTick);
        void Pause(int64_t curTick);
        void Reset();

        bool SetTimer(std::chrono::hours hr, std::chrono::minutes min, std::chrono::seconds sec);

        template<typename Rep, typename Period>
        void Add(std::chrono::duration<Rep, Period> adder)
        {
            duration_ += std::chrono::duration_cast<Tick>(adder).count();
        }

        template<typename Rep, typename Period>
        void Subtract(std::chrono::duration<Rep, Period> adder)
        {
            duration_ -= std::chrono::duration_cast<Tick>(adder).count();
        }

    private:
        void Update(int64_t curTick);

        int64_t goal_{0};

        int64_t duration_{0};
        int64_t latest_{0};

        TimerState state_{Idle};
    };
}

#endif //MANGO_TICKTIMER_H
