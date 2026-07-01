#ifndef MANGO_TESTSTATE_H
#define MANGO_TESTSTATE_H

#include <entt/fwd.hpp>
#include "State/State.h"
#include "Resource/ResourceFwd.h"
#include "Collision/CollisionEventFwd.h"
#include "UIController.h"

class TestState : public tomato::State {
public:
    explicit TestState(tomato::Engine& engine) : State(engine) {}

    void Init() override;
    void Update() override;
    void Exit() override;

private:
    static void TEST_CollisionEnter(const tomato::CollisionEnterEvent& event);
    static void TEST_CollisionExit(const tomato::CollisionExitEvent& event);

    tomato::Audio* audioPtr_{nullptr};
    UIController uiController_;
};

#endif //MANGO_TESTSTATE_H