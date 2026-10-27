//
// Created by yejin on 26. 10. 23..
//

#ifndef MANGO_RENDERTESTSTATE_H
#define MANGO_RENDERTESTSTATE_H

#include "State/State.h"

class RenderTestState : public tomato::State
{
public:
    explicit RenderTestState(tomato::Engine& engine) : State(engine) {}

    void Init() override;
    void Update() override;
    void Exit() override;
};

#endif //MANGO_RENDERTESTSTATE_H