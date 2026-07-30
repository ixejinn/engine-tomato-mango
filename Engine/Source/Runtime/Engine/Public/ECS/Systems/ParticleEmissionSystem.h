#ifndef MANGO_PARTICLEEMISSIONSYSTEM_H
#define MANGO_PARTICLEEMISSIONSYSTEM_H

#include <entt/fwd.hpp>

#include "ECS/Systems/System.h"
#include "ECS/Forward/ParticleCompFwd.h"
#include "ECS/Forward/PhysCompFwd.h"

namespace tomato
{
    class ParticleEmissionSystem : public System
    {
    public:
        void Update(SimContext& simCtx) override;

    private:
        bool ProcessEmitterLifeTime(SimContext& simCtx, entt::entity e, ParticleData& particle);
        void ProcessParticleLifeTime(ParticleData& particle);

        void RateOverTimeParticle(entt::registry& reg, entt::entity e, ParticleData& particle);
        void BurstParticle(entt::registry& reg, entt::entity e, ParticleData& particle);

        void UpdateAliveParticles(ParticleData& particle);

        void InitializeParticles(ParticleData& comp, TransformComponent* transform = nullptr, int num = 1);
    };
}

#endif //MANGO_PARTICLEEMISSIONSYSTEM_H