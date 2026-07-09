#ifndef MANGO_STATE_H
#define MANGO_STATE_H

#include <array>
#include <unordered_map>
#include <entt/entity/registry.hpp>
#include "State/StateFwd.h"
#include "Particle/ParticleEmitterPool.h"
#include "UUID.h"
#include "TomatoFwd.h"

namespace tomato
{
    class State
    {
    public:
        explicit State(Engine& engine);
        virtual ~State() = default;

        virtual void Init() = 0;
        virtual void Update() = 0;
        virtual void Exit() = 0;

        entt::registry& GetRegistry() { return registry_; }

        PlayerInputTimelines& GetPlayerInputTimelines() { return playerInputs_; }

        void SetPlayerInput(uint32_t tick, InputRecord input, int playerID);

    protected:
        Engine& engine_;
        entt::registry registry_;
        std::unordered_map<UUID, entt::entity> entityMap_;

        PlayerInputTimelines playerInputs_;

    public:
        ParticleEmitterPool particlePool_;
    };
}

#endif //MANGO_STATE_H