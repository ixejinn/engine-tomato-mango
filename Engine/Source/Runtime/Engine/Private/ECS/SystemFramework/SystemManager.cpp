#include "ECS/SystemFramework/SystemManager.h"
#include "ECS/SystemFramework/SystemRegistry.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "ECS/Systems/System.h"

namespace tomato
{
    SystemManager::SystemManager()
    {
        auto& sysRegistry = SystemRegistry::GetInstance();

        for (const SystemPhase phase : SIMULATION_ORDER)
        {
            for (const auto& factory : sysRegistry.GetSimFactory(phase))
                systems_[phase].emplace_back(factory());
        }

        for (const SystemPhase phase : RENDERING_ORDER)
        {
            for (const auto& factory : sysRegistry.GetSimFactory(phase))
                systems_[phase].emplace_back(factory());
        }
    }

    SystemManager::~SystemManager() = default;

    void SystemManager::Simulate(SimContext& sim)
    {
        for (const SystemPhase phase : SIMULATION_ORDER)
        {
            for (const auto& system : systems_[phase])
                system->Update(sim);
        }
    }

    void SystemManager::Render(SimContext& sim)
    {
        for (const SystemPhase phase : RENDERING_ORDER)
        {
            for (const auto& system : systems_[phase])
                system->Update(sim);
        }
    }

    void SystemManager::InitializeTransform(SimContext& sim)
    {
        for (const auto& system : systems_[SystemPhase::Transformation])
            system->Update(sim);
    }
}