#include "Particle/ParticleEmitterPool.h"
#include "Particle/ParticleConfig.h"
#include "Resource/AssetRegistry.h"
#include "Resource/Render/ParticleEffect.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Target.h"
#include "ECS/Components/Particle.h"

namespace tomato
{
    namespace
    {
        void ResetEmitter(entt::registry& registry, entt::entity e, AssetID effectID)
        {
            auto& emitter = registry.get<ParticleEmitterComponent>(e);
            auto& sim = registry.get<ParticleSimulationComponent>(e);

            emitter.origin = effectID;

            if (effectID != 0)
            {
                if (const auto* ptc = AssetRegistry<ParticleEffect>::GetInstance().Get(effectID))
                    emitter.sets = ptc->GetSettings();
                else
                {
                    TMT_ERR << "Invalid particle effect asset: " << effectID;
                    emitter.sets = ParticleEmitterSettings{ .duration = 0.0f, .looping = false };    // 다음 틱에 반납
                }
            }
            else
                emitter.sets = ParticleEmitterSettings{};

            emitter.sets.maxParticles = std::min(emitter.sets.maxParticles, static_cast<uint32_t>(sim.positions.size()));
            sim.time = -emitter.sets.startDelay;
            sim.accumulator = 0.0f;
            sim.activeCnt = 0;
        }
    }

    void ParticleEmitterPoolTraits::Assemble(entt::registry& registry, entt::entity e)
    {
        const auto* config = registry.ctx().find<ParticleConfig>();
        const uint32_t capacity = config ? config->maxParticlesPerEmitter : ParticleConfig{}.maxParticlesPerEmitter;

        registry.emplace<ParticleEmitterComponent>(e);
        registry.emplace<TargetComponent>(e);
        auto& sim = registry.emplace<ParticleSimulationComponent>(e);
        sim.positions.resize(capacity);
        sim.velocities.resize(capacity);
        sim.remainingLifetimes.resize(capacity);
    }

    void ParticleEmitterPoolTraits::Reset(entt::registry& registry, entt::entity e,
        glm::vec3 pos, AssetID effectID)
    {
        registry.get<TransformComponent>(e).SetPosition(pos);
        ResetEmitter(registry, e, effectID);
    }

    void ParticleEmitterPoolTraits::Reset(entt::registry& registry, entt::entity e,
        entt::entity target, AssetID effectID, glm::vec3 offset)
    {
        auto& targetComp = registry.get<TargetComponent>(e);
        targetComp.target = target;
        targetComp.headOffset = offset;

        ResetEmitter(registry, e, effectID);
    }

    bool ParticleEmitterPoolTraits::Deactivate(entt::registry& registry, entt::entity e)
    {
        auto* emitter = registry.try_get<ParticleEmitterComponent>(e);
        auto* sim = registry.try_get<ParticleSimulationComponent>(e);
        auto* target = registry.try_get<TargetComponent>(e);
        if (!emitter || !sim || !target)
            return false;

        sim->activeCnt = 0;
        target->target = entt::null;

        return true;
    }
}
