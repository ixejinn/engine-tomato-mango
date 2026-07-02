#ifndef MANGO_TIMER_H
#define MANGO_TIMER_H

#include <chrono>

namespace tomato
{
    class Timer
    {
    public:
        enum State
        {
            Idle,
            Running,
            Paused,
            TimeUp,
            Finished
        };

        Timer() = default;
        Timer(std::chrono::hours hr, std::chrono::minutes min, std::chrono::seconds sec)
        {
            goal_ += (hr + min + sec);
            duration_ = goal_;
        }

        State GetState() const { return state_; }

        [[nodiscard]] bool IsTimeUp();
        std::string GetString(bool padMinutes = false, bool showHours = false);

        void Start();
        void Pause();
        void Reset();

        bool SetTimer(std::chrono::hours hr, std::chrono::minutes min, std::chrono::seconds sec);

        template<typename Rep, typename Period>
        void Add(std::chrono::duration<Rep, Period> adder)
        {
            duration_ += adder;
        }

        template<typename Rep, typename Period>
        void Subtract(std::chrono::duration<Rep, Period> adder)
        {
            duration_ -= adder;
        }

    private:
        void Update();

        std::chrono::milliseconds goal_{0};

        std::chrono::milliseconds duration_{0};
        std::chrono::steady_clock::time_point latest_;

        State state_{Idle};
    };
}

#endif //MANGO_TIMER_H