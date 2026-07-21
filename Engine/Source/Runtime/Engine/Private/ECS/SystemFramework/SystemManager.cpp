#include "ECS/SystemFramework/SystemManager.h"
#include "ECS/SystemFramework/SystemRegistry.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "ECS/Systems/System.h"
#include "Utils/Bitmask/BitmaskOperators.h"

namespace tomato
{
    SystemManager::~SystemManager() = default;

    void SystemManager::AddSystem(TickPhase phase, RunMode mode, SystemPtr system)
    {
        tickSystems_[phase].emplace_back(mode, std::move(system));
    }

    void SystemManager::AddSystem(FramePhase phase, RunMode mode, SystemPtr system)
    {
        frameSystems_[phase].emplace_back(mode, std::move(system));
    }

    void SystemManager::FixedUpdate(SimContext& simCtx, RunMode mode)
    {
        for (auto& systems : tickSystems_)
        {
            for (auto& [sysMode, sysPtr] : systems)
            {
                if (HasFlag(sysMode, mode))
                    sysPtr->Update(simCtx);
            }
        }
    }

    void SystemManager::Update(SimContext& simCtx, RunMode mode)
    {
        for (auto& systems : frameSystems_)
        {
            for (auto& [sysMode, sysPtr] : systems)
            {
                if (HasFlag(sysMode, mode))
                    sysPtr->Update(simCtx);
            }
        }
    }

    void SystemManager::Update(TickPhase phase, SimContext& simCtx, RunMode mode)
    {
        for (auto& [sysMode, sysPtr] : tickSystems_[phase])
        {
            if (HasFlag(sysMode, mode))
                sysPtr->Update(simCtx);
        }
    }

    void SystemManager::Update(FramePhase phase, SimContext& simCtx, RunMode mode)
    {
        for (auto& [sysMode, sysPtr] : frameSystems_[phase])
        {
            if (HasFlag(sysMode, mode))
                sysPtr->Update(simCtx);
        }
    }
}