#ifndef MANGO_COLLISIONTESTSTATE_H
#define MANGO_COLLISIONTESTSTATE_H

#include "State/State.h"

class CollisionTestState : public tomato::State
{
public:
    explicit CollisionTestState(tomato::Engine& engine) : State(engine) {}

    void Init() override;
    void Update() override;
    void Exit() override;

private:

};

#endif //MANGO_COLLISIONTESTSTATE_H