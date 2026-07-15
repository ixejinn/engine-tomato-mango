#ifndef MANGO_PARTICLEEFFECT_H
#define MANGO_PARTICLEEFFECT_H

#include <optional>
#include <glm/vec4.hpp>
#include "ECS/Forward/ParticleCompFwd.h"
#include "Resource/ResourceFwd.h"
#include "Serialization/Json.h"
#include "Particle/ParticleType.h"

namespace tomato
{
    class ParticleEffect
    {
    public:
        struct Burst
        {
            float period;
            int cycles;
            uint8_t count;
        };

    private:
        ParticleEffect(const char* filename);

    public:
        static void Cleanup() {};
        static void Create(const char* filename);

        void InitializeParticleComponent(ParticleComponent& comp) const;

    private:
        float duration_;
        bool looping_;

        float startDelay_;
        float startSpeed_;

        int maxParticles_;

        ParticleEffectShape shape_;
        float angle_;  // for Shape::Cone
        ParticleSimulationSpace space_;

        float lifetime_;

        AssetID texture_;
        float size_;
        glm::vec4 color_;

        uint8_t rateOverTime_;

        std::optional<Burst> burst_;
    };
}

#endif //MANGO_PARTICLEEFFECT_H
