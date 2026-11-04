#ifndef MANGO_PARTICLEEMITTERSETTINGS_H
#define MANGO_PARTICLEEMITTERSETTINGS_H

#include <vector>
#include <cstdint>
#include <glm/vec4.hpp>

#include "Resource/ResourceFwd.h"
#include "Serialization/Json.h"
#include "Particle/ParticleType.h"

namespace tomato
{
    /**
     * @brief 특정 시점에 파티클을 한 번에 생성
     *
     * 이미터 시간이 time + k * interval (0 <= k < cycles)마다 count개의 파티클을 생성
     */
    struct ParticleBurst
    {
        float    time{0};       // 이미터 시작 기준 첫 버스트 시각
        uint32_t count{0};      // 한 번에 생성하는 파티클 수
        uint32_t cycles{1};     // 반복 횟수
        float    interval{0};   // 반복 간격 (0이하이면 cycles번 분량을 time 때 한 번에 생성)
    };

    /**
     * @brief 파티클 이펙트 에셋에 저장되는 데이터
     */
    struct ParticleEmitterSettings
    {
        //// Main
        float    duration{1};       // 이미터 한 사이클의 길이
        bool     looping{true};     // duration 반복 여부
        float    startDelay{0};     // 재생 시작 후 첫 생성까지 대기 시간
        uint32_t maxParticles{100}; // 이미터의 동시 최대 파티클 수
        ParticleSimulationSpace simSpace{ParticleSimulationSpace::World};

        float lifetime{1};  // 파티클의 생명 주기 (입자 하나가 살아있는 기간)
        float speed{1};     // 파티클 초기 속력
        float size{0.1};    // 파티클 빌보드 한 변의 크기
        glm::vec4 color{1};

        //// Emission
        float rateOverTime{0};  // 초당 생성되는 파티클 개수 (하나씩 생성)
        std::vector<ParticleBurst> bursts;

        //// Shape
        ParticleEmitterShape shape{ParticleEmitterShape::Cone};
        float coneAngle{0}; // 파티클 속도의 방향이 +Y축과 이루는 최대 각도

        //// Render
        AssetID texture{0};
    };

    void from_json(const json& j, ParticleBurst& burst);
    void to_json(json& j, const ParticleBurst& burst);

    void from_json(const json& j, ParticleEmitterSettings& sets);
    void to_json(json& j, const ParticleEmitterSettings& sets);
}

#endif //MANGO_PARTICLEEMITTERSETTINGS_H