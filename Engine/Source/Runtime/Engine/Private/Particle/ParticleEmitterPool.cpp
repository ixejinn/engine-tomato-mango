#include <entt/entt.hpp>
#include "Particle/ParticleEmitterPool.h"
#include "ECS/Components/Nametag.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/Particle.h"

namespace tomato
{
    ParticleEmitterPool::ParticleEmitterPool(const PassKey<State>& key, entt::registry& reg, uint8_t poolSize)
        : registry_(reg)
    {
        freeEmitters_.reserve(poolSize);

        for (int i = 0; i < poolSize; ++i)
        {
            entt::entity e = registry_.create();
            registry_.emplace<NametagComponent>(e,
                GenerateUUID(), GenerateEntityName(registry_, "ParticleEffect"));
            registry_.emplace<TransformComponent>(e);
            registry_.emplace<RootEntityTag>(e);

            auto& particle = registry_.emplace<ParticleEffectComponent>(e);
            particle.positions.reserve(ParticleEffectComponent::MAX_PARTICLE);
            particle.velocities.reserve(ParticleEffectComponent::MAX_PARTICLE);
            particle.lifetimes.reserve(ParticleEffectComponent::MAX_PARTICLE);
            particle.scales.reserve(ParticleEffectComponent::MAX_PARTICLE);
        }
    }

    std::optional<entt::entity> ParticleEmitterPool::Acquire(ParticleEffectType type, glm::vec3 pos)
    {
        if (freeEmitters_.empty() || type >= COUNT)
        {
            TMT_DEBUG << "Empty particle emitter pool.";
            return std::nullopt;
        }

        entt::entity e = freeEmitters_.back();
        freeEmitters_.pop_back();

        auto& transform = registry_.get<TransformComponent>(e);
        transform.SetPosition(pos);

        auto& particle = registry_.get<ParticleComponent>(e);
        switch (type)
        {
        case Jump:
            {

            }
            break;

        case Move:
            break;
        default: ;
        }

        return e;
    }

    std::optional<entt::entity> ParticleEmitterPool::Acquire(ParticleEffectType type, entt::entity parent)
    {
        if (freeEmitters_.empty())
        {
            TMT_DEBUG << "Empty particle emitter pool.";
            return std::nullopt;
        }

        entt::entity e = freeEmitters_.back();
        freeEmitters_.pop_back();

        // TODO: Initialize ParticleEffectComponent

        return e;
    }

    bool ParticleEmitterPool::Release(entt::entity e)
    {
        auto* particle = registry_.try_get<ParticleEffectComponent>(e);
        if (!particle)
            return false;

        particle->positions.clear();
        particle->velocities.clear();
        particle->lifetimes.clear();
        particle->scales.clear();
        particle->activeCnt = 0;

        freeEmitters_.push_back(e);
        return true;
    }

    void ParticleEmitterPool::SetParticleEmitter(ParticleEffectType type)
    {

    }
}
