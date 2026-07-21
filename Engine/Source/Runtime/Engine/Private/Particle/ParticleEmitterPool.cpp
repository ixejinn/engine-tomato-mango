#include <entt/entt.hpp>
#include "Particle/ParticleEmitterPool.h"
#include "ECS/Components/Nametag.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Hierarchy.h"
#include "ECS/Components/Particle.h"
#include "ECS/Components/Visibility.h"
#include "ECS/Entity/Hierarchy.h"
#include "Resource/AssetRegistry.h"
#include "Resource/Render/Texture.h"
#include "Resource/Render/ParticleEffect.h"

namespace tomato
{
    ParticleEmitterPool::ParticleEmitterPool(const PassKey<State>& key, entt::registry& reg, uint8_t poolSize)
        : registry_(reg), poolSize_(poolSize)
    {
        freeEmitters_.reserve(poolSize);

        for (int i = 0; i < poolSize; ++i)
        {
            entt::entity e = registry_.create();
            //registry_.emplace<NametagComponent>(e,
            //    GenerateUUID(), GenerateEntityName(registry_, "ParticleEffect"));
            registry_.emplace<TransformComponent>(e);
            registry_.emplace<RootEntityTag>(e);
            //registry_.emplace<VisibilityComponent>(e);

            auto& ptc = registry_.emplace<ParticleComponent>(e);
            ptc.active = false;
            ptc.positions.reserve(ParticleComponent::MAX_PARTICLE);
            ptc.velocities.reserve(ParticleComponent::MAX_PARTICLE);
            ptc.lifetimes.reserve(ParticleComponent::MAX_PARTICLE);

            freeEmitters_.push_back(e);
        }
    }

    std::optional<entt::entity> ParticleEmitterPool::Acquire(AssetID ptcID, glm::vec3 pos)
    {
        if (freeEmitters_.empty())
        {
            TMT_DEBUG << "Empty particle emitter pool.";
            return std::nullopt;
        }

        ParticleEffect* ptcEffect = AssetRegistry<ParticleEffect>::GetInstance().Get(ptcID);

        entt::entity e = freeEmitters_.back();
        freeEmitters_.pop_back();

        auto& transform = registry_.get<TransformComponent>(e);
        transform.SetPosition(pos);

        auto& particle = registry_.get<ParticleComponent>(e);
        particle.active = true;
        ptcEffect->InitializeParticleComponent(particle);

        return e;
    }

    std::optional<entt::entity> ParticleEmitterPool::Acquire(AssetID ptcID, entt::entity parent)
    {
        if (freeEmitters_.empty())
        {
            TMT_DEBUG << "Empty particle emitter pool.";
            return std::nullopt;
        }

        ParticleEffect* ptcEffect = AssetRegistry<ParticleEffect>::GetInstance().Get(ptcID);

        entt::entity e = freeEmitters_.back();
        freeEmitters_.pop_back();

        SetHierarchy(registry_, parent, e);

        auto& particle = registry_.get<ParticleComponent>(e);
        particle.active = true;
        ptcEffect->InitializeParticleComponent(particle);

        return e;
    }

    bool ParticleEmitterPool::Release(entt::entity e)
    {
        auto* particle = registry_.try_get<ParticleComponent>(e);
        if (!particle)
            return false;

        SetHierarchy(registry_, entt::null, e);

        particle->active = false;

        freeEmitters_.push_back(e);
        return true;
    }
}
