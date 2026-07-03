#include "Clock/Timer.h"

using namespace std::chrono_literals;

namespace tomato
{
    bool Timer::IsTimeUp()
    {
        if (state_ == Running)
            Update();

        if (state_ == TimeUp)
        {
            state_ = Finished;
            return true;
        }

        return false;
//        switch (state_)
//        {
//        case Running:
//            Update();
//            break;
//        case TimeUp:
//            state_ = Finished;
//            return true;
//        default:
//            break;
//        }
//        return false;
    }

    std::string Timer::GetString(bool padMinutes, bool showHours)
    {
        Update();

        std::string str;
        std::chrono::hh_mm_ss view(duration_);

        if (showHours || duration_ >= 1h)
            str += std::to_string(view.hours().count()) + ":";

        auto min = view.minutes().count();
        auto sec = view.seconds().count();

        if (padMinutes && min < 10)
            str += '0';
        str += std::to_string(min) + ":";

        if (sec < 10)
            str += '0';
        str += std::to_string(sec);

        return str;
    }

    void Timer::Start()
    {
        state_ = Running;

        latest_ = std::chrono::steady_clock::now();
    }

    void Timer::Pause()
    {
        state_ = Paused;

        duration_ -= std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - latest_);
    }

    void Timer::Reset()
    {
        state_ = Idle;

        duration_ = goal_;
    }

    bool Timer::SetTimer(std::chrono::hours hr, std::chrono::minutes min, std::chrono::seconds sec)
    {
        if (state_ != Paused && state_ != Running)
        {
            goal_ = (hr + min + sec);
            Reset();
            return true;
        }
        return false;
    }

    void Timer::Update()
    {
        if (state_ == Running)
        {
            auto cur = std::chrono::steady_clock::now();
            duration_ -= std::chrono::duration_cast<std::chrono::milliseconds>(cur - latest_);

            if (duration_ <= 0ms)
            {
                state_ = TimeUp;
                duration_ = 0ms;
            }
            else
                latest_ = cur;
        }
    }
}