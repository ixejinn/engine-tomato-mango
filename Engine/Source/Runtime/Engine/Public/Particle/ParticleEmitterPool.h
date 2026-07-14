#ifndef MANGO_PARTICLEEMITTERPOOL_H
#define MANGO_PARTICLEEMITTERPOOL_H

#include <vector>
#include <optional>
#include <entt/fwd.hpp>
#include <glm/vec3.hpp>
#include "Utils/PassKey.h"
#include "State/StateFwd.h"
#include "Particle/ParticleType.h"

namespace tomato
{
    class ParticleEmitterPool
    {
    public:
        ParticleEmitterPool(const PassKey<State>& key, entt::registry& reg, uint8_t poolSize = 32);

        std::optional<entt::entity> Acquire(ParticleEffectType type, glm::vec3 pos);
        std::optional<entt::entity> Acquire(ParticleEffectType type, entt::entity parent);
        bool Release(entt::entity e);

        uint8_t GetActiveEmitterNum() const { return poolSize_ - freeEmitters_.size(); }

    private:
        void SetParticleEmitter(ParticleEffectType type);

        entt::registry& registry_;

        uint8_t poolSize_;
        std::vector<entt::entity> freeEmitters_;
    };
}

#endif //MANGO_PARTICLEEMITTERPOOL_H