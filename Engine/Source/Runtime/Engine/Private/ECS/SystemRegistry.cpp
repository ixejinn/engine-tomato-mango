#include "ECS/SystemRegistry.h"

namespace tomato {
    void SystemRegistry::RegisterFactory(SystemPhase phase, SystemFactory&& factory) {
        factories_[phase].push_back(std::move(factory));
    }

    const std::vector<SystemFactory>& SystemRegistry::GetSimFactory(SystemPhase phase) {
        return factories_[phase];
    }
}