#include "ECS/SystemManager.h"
#include "ECS/SystemRegistry.h"
#include "ECS/SystemUpdateContexts.h"
#include "ECS/Systems/System.h"

namespace tomato {
    SystemManager::SystemManager() {
        auto& sysRegistry = SystemRegistry::GetInstance();

        for (const SystemPhase phase : simOrder_) {
            for (const auto& factory : sysRegistry.GetSimFactory(phase))
                systems_[phase].emplace_back(factory());
        }

        for (const SystemPhase phase : renderOrder_) {
            for (const auto& factory : sysRegistry.GetSimFactory(phase))
                systems_[phase].emplace_back(factory());
        }
    }

    SystemManager::~SystemManager() = default;

    void SystemManager::Simulate(SimContext& sim, InputContext& input) {
        sim.registry.ctx().insert_or_assign<InputContext*>(&input);

        for (const SystemPhase phase : simOrder_) {
            for (const auto& system : systems_[phase])
                system->Update(sim);
        }
    }

    void SystemManager::Render(SimContext& sim, RenderContext& render) {
        sim.registry.ctx().insert_or_assign<RenderContext*>(&render);

        for (const SystemPhase phase : renderOrder_) {
            for (const auto& system : systems_[phase])
                system->Update(sim);
        }
    }
}