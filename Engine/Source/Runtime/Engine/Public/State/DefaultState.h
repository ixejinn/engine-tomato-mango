#ifndef MANGO_DEFAULTSTATE_H
#define MANGO_DEFAULTSTATE_H

#include "State/State.h"

namespace tomato {
    class DefaultState : public State {
    public:
        void Init() override {};
        void Exit() override {};
    };
}

#endif //MANGO_DEFAULTSTATE_H