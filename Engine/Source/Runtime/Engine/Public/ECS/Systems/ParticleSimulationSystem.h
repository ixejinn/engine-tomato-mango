#ifndef MANGO_PARTICLESIMULATIONSYSTEM_H
#define MANGO_PARTICLESIMULATIONSYSTEM_H

#include <vector>
#include <entt/fwd.hpp>

#include "ECS/Systems/System.h"
#include "ECS/Forward/ParticleCompFwd.h"
#include "ECS/Forward/PhysCompFwd.h"
#include "Particle/ParticleFwd.h"

namespace tomato
{
    class ParticleSimulationSystem : public System
    {
    public:
        ParticleSimulationSystem();

        void Update(SimContext& simCtx) override;

    private:
        static void UpdateParticles(ParticleSimulationComponent& sim);

        static void UpdateEmitter(const ParticleEmitterSettings& sets, ParticleSimulationComponent& sim,
            const TransformComponent& trf);

        /**
         * 이미터 시간 [from, to) 동안 생성되어야 하는 파티클 활성화
         */
        static void EmitInWindow(const ParticleEmitterSettings& sets, ParticleSimulationComponent& sim,
                                 const TransformComponent& trf, float from, float to);

        /**
         * @param count 생성할 파티클 개수
         */
        static void Emit(const ParticleEmitterSettings& sets, ParticleSimulationComponent& sim,
                         const TransformComponent& trf, uint32_t count);

        std::vector<entt::entity> finishedEmitters_;
    };
}

#endif //MANGO_PARTICLESIMULATIONSYSTEM_H