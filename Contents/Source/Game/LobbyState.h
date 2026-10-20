#ifndef MANGO_LOBBYSTATE_H
#define MANGO_LOBBYSTATE_H

#include "State/State.h"
#include "PlayTest/UIController.h"

class LobbyState : public tomato::State {
public:
    explicit LobbyState(tomato::Engine& engine);
    ~LobbyState() override;

    void Init() override;
    void Update() override;
    void Exit() override;

private:
    UIController uiController_;
};

#endif //MANGO_LOBBYSTATE_H