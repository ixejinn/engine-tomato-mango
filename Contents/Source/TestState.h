#ifndef MANGO_TESTSTATE_H
#define MANGO_TESTSTATE_H

#include "State/State.h"
#include "Resource/ResourceFwd.h"

class TestState : public tomato::State {
public:
    explicit TestState(tomato::Engine& engine) : State(engine) {}

    void Init() override;
    void Update() override;
    void Exit() override;

private:
    tomato::Audio* audioPtr_;
};

#endif //MANGO_TESTSTATE_H