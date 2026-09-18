#ifndef MANGO_MYSTATE_H
#define MANGO_MYSTATE_H

#include <entt/fwd.hpp>
#include "State/State.h"
#include "Resource/ResourceFwd.h"
#include "Collision/CollisionEventFwd.h"
#include "../UIController.h"
#include "GameObject/Character/MovementMode.h"

class MyState : public tomato::State {
public:
    explicit MyState(tomato::Engine& engine) : State(engine) {}

    void Init() override;
    void Update() override;
    void Exit() override;

private:

    tomato::Audio* audioPtr_{ nullptr };
    UIController uiController_;

    static void WaveCollisionEnter(const tomato::TriggerEnterEvent& event);
    static void WaveCollisionExit(const tomato::TriggerExitEvent& event);
    static void MakeWaveJump(const tomato::LandingEvent& event);
};

#endif //MANGO_MYSTATE_H