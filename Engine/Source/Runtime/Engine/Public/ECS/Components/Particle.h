#ifndef MANGO_PARTICLE_H
#define MANGO_PARTICLE_H

#include <vector>
#include <chrono>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "Resource/ResourceFwd.h"

namespace tomato
{
    struct ParticleEffectComponent
    {
        inline static uint8_t MAX_PARTICLE = 64;

        struct ParticleLifetime
        {
            std::chrono::milliseconds duration;
            std::chrono::steady_clock::time_point start;
        };

        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> velocities;
        std::vector<ParticleLifetime> lifetimes;
        std::vector<float> scales;

        AssetID texture;
        glm::vec4 color{1.f};

        int activeCnt = 0;
    };

    struct ParticleComponent
    {
        inline static int MAX_PARTICLE = 64;

        struct Lifetime
        {
            std::chrono::milliseconds duration;
            std::chrono::steady_clock::time_point start;
        };

        bool active{false};

        Lifetime emitter;
        bool looping;
        ParticleEffectShape shape;
        float angle;

        std::chrono::milliseconds emitPeriod;
        std::chrono::steady_clock::time_point latestTP;

        std::chrono::milliseconds startDelay;
        float startSpeed;

        AssetID texture;
        float size;
        glm::vec4 color;

        // each particle
        std::chrono::milliseconds lifetime;

        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> velocities;
        std::vector<Lifetime> lifetimes;

        int activeCnt = 0;
        uint8_t maxParticles;
    };
}

#endif //MANGO_PARTICLE_H