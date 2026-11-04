#ifndef MANGO_PARTICLE_H
#define MANGO_PARTICLE_H

#include "Resource/ResourceFwd.h"
#include "Particle/ParticleEmitterSettings.h"

namespace tomato
{
    struct ParticleEmitterComponent
    {
        AssetID origin{0};              // 원본 에셋
        ParticleEmitterSettings sets;   // 원본 에셋 복사본
    };

    struct ParticleSimulationComponent
    {
        // Emitter
        float time{0};  // 현재 사이클 기준 이미터 시간(초)
        float accumulator{0};

        // Particle data
        uint32_t activeCnt{0};  // 활동 중인 파티클 개수
        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> velocities;
        std::vector<float> remainingLifetimes;  // 남은 수명(초), 0 이하가 되면 소멸
    };
}

#endif //MANGO_PARTICLE_H