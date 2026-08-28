#include "Engine.h"
#include "State/State.h"
#include "State/StateRegistry.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "ECS/Entity/Entity.h"
#include "ECS/Components/EditorTag.h"
#include "Particle/ParticleEmitterPool.h"
#include "Prefab/Prefab.h"
#include "Utils/PassKey.h"

namespace tomato
{
    State::State(Engine& engine) : engine_(engine)
    {
        registry_.ctx().emplace<RenderContext>();
        registry_.ctx().emplace<CollisionContext>();
        registry_.ctx().emplace<UIContext>();
        registry_.ctx().emplace<EntityNameGenerator>();
        registry_.ctx().emplace<ParticleEmitterPool>(PassKey<State>(), registry_);

        //// Set render context
        auto& renderCtx = registry_.ctx().get<RenderContext>();

        // Create editor mode camera
        entt::entity& editCam = renderCtx.editorCam;
        editCam = Prefab::CreateCamera(registry_, "Edit Camera", false);
        registry_.emplace<EditorHidden>(editCam);

        // Skybox
        entt::entity& skybox = renderCtx.skybox;
        skybox = Prefab::CreateSkybox(registry_);
        registry_.emplace<NoInspector>(skybox);

        // View gizmo
        entt::entity& viewGizmo = renderCtx.viewGizmo;
        viewGizmo = Prefab::CreateGizmo(registry_);
    }

    void State::SetNextState(std::unique_ptr<State>&& newState)
    {
        engine_.SetNextState(std::move(newState));
    }

    std::unique_ptr<State> State::GetStateByID(AssetID stateID)
    {
        return StateRegistry::GetInstance().GetStateFactory(stateID)(engine_);
    }

    Window& State::GetWindow()
    {
        return engine_.GetWindow();
    }

    void State::SetPlayerInput(uint32_t tick, IntentState input, int playerID)
    {
        playerInputs_[playerID].SetData(tick, input);
    }
}