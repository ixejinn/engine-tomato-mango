#ifndef MANGO_PARTICLECONFIG_H
#define MANGO_PARTICLECONFIG_H

#include <cstdint>
#include "Serialization/Json.h"

namespace tomato
{
    struct ParticleConfig
    {
        uint32_t maxParticlesPerEmitter{256};
    };

    inline void from_json(const json& j, ParticleConfig& cfg)
    {
        cfg.maxParticlesPerEmitter = j.at("maxParticlesPerEmitter").get<uint32_t>();
    }

    inline void to_json(json& j, const ParticleConfig& cfg)
    {
        j["maxParticlesPerEmitter"] = cfg.maxParticlesPerEmitter;
    }
}

#endif //MANGO_PARTICLECONFIG_H