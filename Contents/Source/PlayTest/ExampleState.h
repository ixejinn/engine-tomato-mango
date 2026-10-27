#ifndef MANGO_EXAMPLESTATE_H
#define MANGO_EXAMPLESTATE_H

#include <glm/vec4.hpp>
#include "State/State.h"
#include "GameObject/Character/MovementMode.h"
#include "Resource/ResourceFwd.h"

class ExampleState : public tomato::State
{
public:
    explicit ExampleState(tomato::Engine& engine) : State(engine) {}

    void Init() override;
    void Update() override;
    void Exit() override {};

private:
    void CallbackLand(const tomato::LandingEvent& event);

    void ChangeMesh(entt::entity e, tomato::AssetID mesh, bool character);

    tomato::Audio* sfxPtr{nullptr};
    tomato::AssetID jumpPtcl;

    const glm::vec4 omoi = glm::vec4(28/255.f, 216/255.f, 232/255.f, 1);
    const glm::vec4 karui = glm::vec4(255/255.f, 34/255.f, 19/255.f, 0.5);

    tomato::AssetID cubeID;
    tomato::AssetID sphereID;
};

#endif //MANGO_EXAMPLESTATE_H