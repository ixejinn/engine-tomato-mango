#ifndef MANGO_PARTICLETYPE_H
#define MANGO_PARTICLETYPE_H

#include <cstdint>
#include <chrono>
#include "Containers/EnumArray.h"

namespace tomato
{
#define TMT_PARTICLE_EFFECT_SHAPE_LIST(X)   \
        X(Sphere, "Sphere")                 \
        X(Circle, "Circle")                 \
        X(Cone, "Cone")

    enum class ParticleEffectShape
    {
#define X(Enum, Display) Enum,
        TMT_PARTICLE_EFFECT_SHAPE_LIST(X)
#undef X
    };

    struct ParticleEffectShapeMeta
    {
        ParticleEffectShape shape;
        const char* name;
    };

    static constexpr ParticleEffectShapeMeta shapeMetas[]
            {
#define X(Enum, Display) {ParticleEffectShape::Enum, Display},
                    TMT_PARTICLE_EFFECT_SHAPE_LIST(X)
#undef X
            };
#undef TMT_PARTICLE_EFFECT_SHAPE_LIST

    NLOHMANN_JSON_SERIALIZE_ENUM(
            ParticleEffectShape,
            {
                {ParticleEffectShape::Sphere, "Sphere"},
                {ParticleEffectShape::Circle, "Circle"},
                {ParticleEffectShape::Cone, "Cone"}
            }
    )



    enum ParticleEffectType
    {
        Jump,
        Move,
        COUNT
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