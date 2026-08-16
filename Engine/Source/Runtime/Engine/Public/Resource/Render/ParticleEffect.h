#ifndef MANGO_PARTICLEEFFECT_H
#define MANGO_PARTICLEEFFECT_H

#include <optional>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <filesystem>
#include "ECS/Forward/ParticleCompFwd.h"
#include "Resource/ResourceFwd.h"
#include "Serialization/Json.h"
#include "Particle/ParticleType.h"
#include "UUID.h"

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
        ParticleEffect(const std::filesystem::path& path);

    public:
        static void Cleanup() {};
        static void Create(const std::filesystem::path& path);

        void InitializeParticleComponent(ParticleData& comp) const;

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

        /*UUID target_;
        glm::vec3 offset_;*/
    };
}

#endif //MANGO_PARTICLEEFFECT_H
