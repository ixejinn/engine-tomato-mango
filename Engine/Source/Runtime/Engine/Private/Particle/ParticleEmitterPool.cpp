#include <entt/entt.hpp>
#include "Particle/ParticleEmitterPool.h"
#include "ECS/Components/Nametag.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Target.h"
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

            auto& generator = reg.ctx().get<EntityNameGenerator>();
            registry_.emplace<NametagComponent>(e,
                GenerateUUID(), generator.Generate("ParticleEffect"));
            registry_.emplace<VisibilityComponent>(e);

            registry_.emplace<TransformComponent>(e);
            registry_.emplace<RootEntityTag>(e);

            registry_.emplace<ParticleEmitterComponent>(e);
            registry_.emplace<ParticleRuntimeComponent>(e);
            registry_.emplace<ParticleRenderComponent>(e);
            registry_.emplace<TargetComponent>(e);

            auto& particleBuf = registry_.emplace<ParticleBufferComponent>(e);
            particleBuf.positions.reserve(MAX_PARTICLE_NUM);
            particleBuf.velocities.reserve(MAX_PARTICLE_NUM);
            particleBuf.lifetimes.reserve(MAX_PARTICLE_NUM);

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

        auto& pRuntime = registry_.get<ParticleRuntimeComponent>(e);
        pRuntime.active = true;

        auto& attach = registry_.emplace<ParticleAttachmentComponent>(e);
        attach.particle = ptcID;

        ParticleData pData{
            .emitter = registry_.get<ParticleEmitterComponent>(e),
            .runtime = pRuntime,
            .buffer = registry_.get<ParticleBufferComponent>(e),
            .render = registry_.get<ParticleRenderComponent>(e)
        };

        ptcEffect->InitializeParticleComponent(pData);

        return e;
    }

    std::optional<entt::entity> ParticleEmitterPool::Acquire(AssetID ptcID, UUID target, glm::vec3 offset)
    {
        if (freeEmitters_.empty())
        {
            TMT_DEBUG << "Empty particle emitter pool.";
            return std::nullopt;
        }

        ParticleEffect* ptcEffect = AssetRegistry<ParticleEffect>::GetInstance().Get(ptcID);

        entt::entity e = freeEmitters_.back();
        freeEmitters_.pop_back();

        auto& pRuntime = registry_.get<ParticleRuntimeComponent>(e);
        pRuntime.active = true;

        auto& pTarget = registry_.get<TargetComponent>(e);
        pTarget.target = target;
        pTarget.headOffset = offset;

        auto& attach = registry_.emplace<ParticleAttachmentComponent>(e);
        attach.particle = ptcID;
        attach.target = target;

        ParticleData pData{
            .emitter = registry_.get<ParticleEmitterComponent>(e),
            .runtime = pRuntime,
            .buffer = registry_.get<ParticleBufferComponent>(e),
            .render = registry_.get<ParticleRenderComponent>(e)
        };

        ptcEffect->InitializeParticleComponent(pData);

        return e;
    }

    bool ParticleEmitterPool::Release(entt::entity e)
    {
        auto* particle = registry_.try_get<ParticleRuntimeComponent>(e);
        if (!particle)
            return false;

        registry_.remove<ParticleAttachmentComponent>(e);

        particle->active = false;

        auto& pTarget = registry_.get<TargetComponent>(e);
        pTarget.target = 0;

        freeEmitters_.push_back(e);
        return true;
    }
}
