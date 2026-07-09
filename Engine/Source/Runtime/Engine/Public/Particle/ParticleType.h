#ifndef MANGO_PARTICLETYPE_H
#define MANGO_PARTICLETYPE_H

#include <cstdint>
#include <chrono>
#include "Containers/EnumArray.h"

namespace tomato
{
    enum ParticleEffectType
    {
        Jump,
        Move,
        COUNT
    };

    enum ParticleEffectShape
    {
        Sphere,
        Hemisphere,
        Cone
    };

    struct ParticleEffectDetails
    {
        ParticleEffectDetails() = default;  /////
        ParticleEffectDetails(
                int durationMin, int durationMax,
                float scaleMin, float scaleMax,
                ParticleEffectShape shape, float radius,
                uint8_t spawnNum, bool looping, bool gravity)
        : duration(durationMin, durationMax)
        , scale(scaleMin, scaleMax)
        , shape(shape), radius(radius)
        , spawnNum(spawnNum), looping(looping), gravity(gravity) {}

        // Random [min, max]
        std::pair<std::chrono::milliseconds, std::chrono::milliseconds> duration{};
        std::pair<float, float> scale{};

        ParticleEffectShape shape{};
        float radius{};
        float angle{};    // only for Cone shape

        uint8_t spawnNum{};
        bool looping{};
        bool gravity{};
    };

    struct ParticleEffectMap
    {
        inline static EnumArray<ParticleEffectType, ParticleEffectDetails> ParticleDetails =
        {
                {Jump, ParticleEffectDetails(1000, 1000, 0.1, 0.1, Hemisphere, 0.5, 6, false, false)},
                {Move, ParticleEffectDetails(1000, 1000, 0.1, 0.1, Hemisphere, 0.5, 6, false, false)}
        };
    };
}

#endif //MANGO_PARTICLETYPE_H