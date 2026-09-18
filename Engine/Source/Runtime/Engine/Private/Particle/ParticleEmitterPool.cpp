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
    void ParticlePoolTraits::Assemble(entt::registry& registry_, entt::entity e)
    {
        auto& nameTag = registry_.get<NametagComponent>(e);
        nameTag.name = registry_.ctx().get<EntityNameGenerator>().Generate("ParticleEffect");

        registry_.emplace<ParticleEmitterComponent>(e);
        registry_.emplace<ParticleRuntimeComponent>(e);
        registry_.emplace<ParticleRenderComponent>(e);
        registry_.emplace<TargetComponent>(e);

        auto& particleBuf = registry_.emplace<ParticleBufferComponent>(e);
        particleBuf.positions.reserve(MAX_PARTICLE_NUM);
        particleBuf.velocities.reserve(MAX_PARTICLE_NUM);
        particleBuf.lifetimes.reserve(MAX_PARTICLE_NUM);
    }

    void ParticlePoolTraits::Reset(entt::registry& registry_, entt::entity e,
        AssetID ptcID, glm::vec3 pos)
    {
        ParticleEffect* ptcEffect = AssetRegistry<ParticleEffect>::GetInstance().Get(ptcID);

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
    }

    void ParticlePoolTraits::Reset(entt::registry& registry_, entt::entity e,
        AssetID ptcID, UUID target, glm::vec3 offset)
    {
        ParticleEffect* ptcEffect = AssetRegistry<ParticleEffect>::GetInstance().Get(ptcID);

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
    }

    bool ParticlePoolTraits::Deactivate(entt::registry& registry_, entt::entity e)
    {
        auto* particle = registry_.try_get<ParticleRuntimeComponent>(e);
        if (!particle)
            return false;

        registry_.remove<ParticleAttachmentComponent>(e);

        particle->active = false;

        auto& pTarget = registry_.get<TargetComponent>(e);
        pTarget.target = 0;

        return true;
    }
}
