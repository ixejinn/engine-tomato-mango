#include "ECS/SystemFramework/SystemRegistry.h"
#include "ECS/SystemFramework/SystemManager.h"
#include "ECS/SystemFramework/ChangeRunModeEvent.h"
#include "ECS/Systems/Systems.h"
#include "Utils/Bitmask/BitmaskOperators.h"
#include "Event/EventDispatcher.h"
#include "Collision/CollisionEvent.h"
#include "GameObject/Character/CharacterMovement.h"

namespace tomato
{
    void SystemRegistry::RegisterSystems(SystemManager& manager)
    {
        ////// Register tick phase systems
        // Pre
        for (const auto& factory : tickFactories_[TickPhase::PreUpdate])
            manager.AddSystem(TickPhase::PreUpdate, factory.mode, factory.factory());

        // Update
        manager.AddSystem(TickPhase::Update, RunMode::Game | RunMode::Rollback,
            std::make_unique<KinematicMovementSystem>());
        manager.AddSystem(TickPhase::Update, RunMode::Game | RunMode::Rollback,
            std::make_unique<GravitySystem>());
        for (const auto& factory : tickFactories_[TickPhase::Update])
            manager.AddSystem(TickPhase::Update, factory.mode, factory.factory());
        manager.AddSystem(TickPhase::Update, RunMode::Game | RunMode::Rollback,
            std::make_unique<CollisionSystem>());
        manager.AddSystem(TickPhase::Update, RunMode::Game | RunMode::Rollback,
            std::make_unique<IntegrationSystem>());

        // Post
        for (const auto& factory : tickFactories_[TickPhase::PostUpdate])
            manager.AddSystem(TickPhase::PostUpdate, factory.mode, factory.factory());
        manager.AddSystem(TickPhase::PostUpdate, RunMode::Game | RunMode::Editor | RunMode::Rollback,
            std::make_unique<TransformSystem>());

        ////// Register frame phase systems
        // Pre
        manager.AddSystem(FramePhase::PreRender, RunMode::Editor, std::make_unique<EditorCameraSystem>());
        manager.AddSystem(FramePhase::PreRender, RunMode::Game, std::make_unique<PlayerCameraSystem>());
        manager.AddSystem(FramePhase::PreRender, RunMode::Game | RunMode::Editor,
            std::make_unique<CameraTransformSystem>());
        for (const auto& factory : frameFactories_[FramePhase::PreRender])
            manager.AddSystem(FramePhase::PreRender, factory.mode, factory.factory());
        manager.AddSystem(FramePhase::PreRender, RunMode::Game | RunMode::Editor,
            std::make_unique<ParticleEmissionSystem>());

        manager.AddSystem(FramePhase::Render, RunMode::Game | RunMode::Editor,
            std::make_unique<RenderSystem>());
        manager.AddSystem(FramePhase::Render, RunMode::Game | RunMode::Editor,
            std::make_unique<ParticleRenderSystem>());
        for (const auto& factory : frameFactories_[FramePhase::Render])
            manager.AddSystem(FramePhase::Render, factory.mode, factory.factory());

        // UI
        manager.AddSystem(FramePhase::UI, RunMode::Game | RunMode::Editor,
            std::make_unique<UISystem>());
        manager.AddSystem(FramePhase::UI, RunMode::Game | RunMode::Editor,
            std::make_unique<UITransformSystem>());
        manager.AddSystem(FramePhase::UI, RunMode::Game | RunMode::Editor,
            std::make_unique<ScreenUIRenderSystem>());
        manager.AddSystem(FramePhase::UI, RunMode::Game | RunMode::Editor,
            std::make_unique<WorldUIRenderSystem>());
        for (const auto& factory : frameFactories_[FramePhase::UI])
            manager.AddSystem(FramePhase::UI, factory.mode, factory.factory());

        // Post
        for (const auto& factory : frameFactories_[FramePhase::PostRender])
            manager.AddSystem(FramePhase::PostRender, factory.mode, factory.factory());
    }

    void SystemRegistry::RegisterEventCallbacks()
    {
        auto& eventDispatcher = EventDispatcher::GetInstance();
        eventDispatcher.Connect<CollisionEnterEvent>();
        eventDispatcher.Connect<CollisionStayEvent>();
        eventDispatcher.Connect<CollisionExitEvent>();

        eventDispatcher.Connect<TriggerEnterEvent>();
        eventDispatcher.Connect<TriggerStayEvent>();
        eventDispatcher.Connect<TriggerExitEvent>();

        eventDispatcher.Connect<TriggerEnterEvent, &CharacterMovement::OnTriggerEnter_UpdateMovementMode>();
        eventDispatcher.Connect<TriggerExitEvent, &CharacterMovement::OnTriggerExit_UpdateMovementMode>();

        eventDispatcher.Connect<ChangeRunModeEvent>();

        eventDispatcher.Connect<LandingEvent>();
        eventDispatcher.Connect<ChangeMovementModeEvent>();
    }

    void SystemRegistry::RegisterFactory(TickPhase phase, RunMode mode, SystemFactory&& factory)
    {
        tickFactories_[phase].emplace_back(mode, std::move(factory));
    }

    void SystemRegistry::RegisterFactory(FramePhase phase, RunMode mode, SystemFactory&& factory) {
        frameFactories_[phase].emplace_back(mode, std::move(factory));
    }
}