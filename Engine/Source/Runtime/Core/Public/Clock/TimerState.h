#ifndef MANGO_TIMERSTATE_H
#define MANGO_TIMERSTATE_H

namespace tomato
{
    enum TimerState
    {
        Idle,
        Running,
        Paused,
        TimeUp,
        Finished
    };
}

#endif //MANGO_TIMERSTATE_H
