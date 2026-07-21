#include "ECS/SystemFramework/SystemRegistry.h"
#include "ECS/SystemFramework/SystemManager.h"
#include "ECS/Systems/Systems.h"
#include "Utils/Bitmask/BitmaskOperators.h"

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
        manager.AddSystem(FramePhase::PreRender, RunMode::Game | RunMode::Editor,
            std::make_unique<CameraSystem>());
        for (const auto& factory : frameFactories_[FramePhase::PreRender])
            manager.AddSystem(FramePhase::PreRender, factory.mode, factory.factory());
        // TODO: Register particle logic system

        // Render
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
        for (const auto& factory : frameFactories_[FramePhase::UI])
            manager.AddSystem(FramePhase::UI, factory.mode, factory.factory());

        // Post
        for (const auto& factory : frameFactories_[FramePhase::PostRender])
            manager.AddSystem(FramePhase::PostRender, factory.mode, factory.factory());
    }

    void SystemRegistry::RegisterFactory(TickPhase phase, RunMode mode, SystemFactory&& factory)
    {
        tickFactories_[phase].emplace_back(mode, std::move(factory));
    }

    void SystemRegistry::RegisterFactory(FramePhase phase, RunMode mode, SystemFactory&& factory) {
        frameFactories_[phase].emplace_back(mode, std::move(factory));
    }
}