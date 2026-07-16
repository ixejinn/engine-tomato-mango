#ifndef MANGO_PARTICLE_H
#define MANGO_PARTICLE_H

#include <vector>
#include <chrono>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "Resource/ResourceFwd.h"
#include "Particle/ParticleType.h"

namespace tomato
{
    struct ParticleComponent
    {
        inline static int MAX_PARTICLE = 256;

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
        ParticleSimulationSpace space;

        std::chrono::milliseconds emitPeriod;
        std::chrono::milliseconds adder;
        std::chrono::steady_clock::time_point latestTP;

        struct Burst
        {
            std::chrono::milliseconds period;
            std::chrono::milliseconds adder;
            std::chrono::steady_clock::time_point latest;

            int cycles;
            int finishedCycles;

            uint8_t count;
        };
        std::optional<Burst> burst;

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
        int maxParticles;
    };
}

#endif //MANGO_PARTICLE_H