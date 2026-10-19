#ifndef MANGO_MYSTATE_H
#define MANGO_MYSTATE_H

#include "State/State.h"
#include "Collision/CollisionEventFwd.h"
#include "PlayTest/UIController.h"
#include "GameObject/Character/MovementMode.h"

#include "Wave/WaveEventFwd.h"

class GameState : public tomato::State {
public:
    explicit GameState(tomato::Engine& engine);
    ~GameState() override;

    void Init() override;
    void Update() override;
    void Exit() override;

private:
    static void OnTriggerEnter(const tomato::TriggerEnterEvent& event);
    static void OnTriggerExit(const tomato::TriggerExitEvent& event);

    static void OnWaveCollision(const WaveCollisionEvent& event);

    void MakeWaveJump(const tomato::LandingEvent& event);

    UIController uiController_;
};

#endif //MANGO_MYSTATE_H