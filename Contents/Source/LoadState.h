#ifndef MANGO_LOADSTATE_H
#define MANGO_LOADSTATE_H

#include <entt/fwd.hpp>
#include "State/State.h"
#include "Resource/ResourceFwd.h"
#include "Collision/CollisionEventFwd.h"

class LoadState : public tomato::State {
public:
    explicit LoadState(tomato::Engine& engine) : State(engine) {}

    void Init() override;
    void Update() override;
    void Exit() override;

private:
    static void TEST_CollisionEnter(const tomato::CollisionEnterEvent& event, entt::entity e);
    static void TEST_CollisionExit(const tomato::CollisionExitEvent& event, entt::entity e);

    static void TEST_TriggerEnter(const tomato::TriggerEnterEvent& event, entt::entity e);
    static void TEST_TriggerExit(const tomato::TriggerExitEvent& event, entt::entity e);

    tomato::Audio* audioPtr_{ nullptr };
};

#endif //MANGO_LOADSTATE_H