#ifndef MANGO_TESTSTATE_H
#define MANGO_TESTSTATE_H

#include "State/State.h"

class TestState : public tomato::State {
public:
    void Init() override;
    void Exit() override;
};

#endif //MANGO_TESTSTATE_H