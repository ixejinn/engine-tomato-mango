#ifndef MANGO_PARTICLEEFFECT_H
#define MANGO_PARTICLEEFFECT_H

#include <filesystem>
#include "Particle/ParticleEmitterSettings.h"


namespace tomato
{
    class ParticleEffect
    {
    private:
        ParticleEffect(const std::filesystem::path& path);

    public:
        static void Cleanup() {};
        static void Create(const std::filesystem::path& path);

        const ParticleEmitterSettings& GetSettings() const { return sets_; }

    private:
        ParticleEmitterSettings sets_;
    };
}

#endif //MANGO_PARTICLEEFFECT_H
