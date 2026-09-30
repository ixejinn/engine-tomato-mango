#ifndef MANGO_MYSTATE_H
#define MANGO_MYSTATE_H

#include "State/State.h"
#include "Collision/CollisionEventFwd.h"
#include "PlayTest/UIController.h"
#include "GameObject/Character/MovementMode.h"

class GameState : public tomato::State {
public:
    explicit GameState(tomato::Engine& engine);
    ~GameState() override;

    void Init() override;
    void Update() override;
    void Exit() override;

private:
    UIController uiController_;

    static void WaveCollisionEnter(const tomato::TriggerEnterEvent& event);
    static void WaveCollisionExit(const tomato::TriggerExitEvent& event);
    void MakeWaveJump(const tomato::LandingEvent& event);
};

#endif //MANGO_MYSTATE_H