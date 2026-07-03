#include "Clock/TickTimer.h"

namespace tomato
{
    TickTimer::TickTimer(std::chrono::hours hr, std::chrono::minutes min, std::chrono::seconds sec)
    {
        goal_ +=
                std::chrono::duration_cast<Tick>(hr).count() +
                std::chrono::duration_cast<Tick>(min).count() +
                std::chrono::duration_cast<Tick>(sec).count();

        duration_ = goal_;
    }

    bool TickTimer::IsTimeUp(int64_t curTick)
    {
        if (state_ == Running)
            Update(curTick);

        if (state_ == TimeUp)
        {
            state_ = Finished;
            return true;
        }

        return false;
    }

    std::string TickTimer::GetString(int64_t curTick, bool padMinutes, bool showHours)
    {
        Update(curTick);

        std::string str;
        std::chrono::hh_mm_ss view{Tick{duration_}};

        auto hr = view.hours().count();
        auto min = view.minutes().count();
        auto sec = view.seconds().count();

        if (showHours || hr >= 1)
            str += std::to_string(hr) + ":";

        if (padMinutes && min < 10)
            str += '0';
        str += std::to_string(min) + ":";

        if (sec < 10)
            str += '0';
        str += std::to_string(sec);

        return str;
    }

    void TickTimer::Start(int64_t curTick)
    {
        state_ = Running;
        latest_ = curTick;
    }

    void TickTimer::Pause(int64_t curTick)
    {
        state_ = Paused;
        duration_ -= (curTick - latest_);
    }

    void TickTimer::Reset()
    {
        state_ = Idle;
        duration_ = goal_;
    }

    bool TickTimer::SetTimer(std::chrono::hours hr, std::chrono::minutes min, std::chrono::seconds sec)
    {
        if (state_ != Paused && state_ != Running)
        {
            goal_ +=
                    std::chrono::duration_cast<Tick>(hr).count() +
                    std::chrono::duration_cast<Tick>(min).count() +
                    std::chrono::duration_cast<Tick>(sec).count();
            Reset();
            return true;
        }
        return false;
    }

    void TickTimer::Update(int64_t curTick)
    {
        if (state_ == Running)
        {
            duration_ -= (curTick - latest_);

            if (duration_ <= 0)
            {
                state_ = TimeUp;
                duration_ = 0;
            }
            else
                latest_ = curTick;
        }
    }
}