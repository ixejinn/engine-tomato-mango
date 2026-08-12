#include "State/State.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "Particle/ParticleEmitterPool.h"
#include "Utils/PassKey.h"
#include "Prefab/Prefab.h"

namespace tomato
{
    State::State(Engine& engine) : engine_(engine)
    {
        registry_.ctx().emplace<RenderContext>();
        registry_.ctx().emplace<CollisionContext>();
        registry_.ctx().emplace<ParticleEmitterPool>(PassKey<State>(), registry_);

        // Create editor mode camera
        entt::entity& editCam = registry_.ctx().get<RenderContext>().editCam;
        editCam = Prefab::CreateCamera(registry_, false);
    }

    void State::SetPlayerInput(uint32_t tick, InputRecord input, int playerID)
    {
        playerInputs_[playerID].SetData(tick, input);
    }
}