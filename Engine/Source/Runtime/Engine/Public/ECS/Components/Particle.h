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
    inline static int MAX_PARTICLE_NUM = 256;

    struct Lifetime
    {
        std::chrono::milliseconds duration;
        std::chrono::steady_clock::time_point start;
    };

    struct Burst
    {
        std::chrono::milliseconds period;
        std::chrono::milliseconds adder;
        std::chrono::steady_clock::time_point latest;

        int cycles;
        int finishedCycles;

        uint8_t count;
    };

    struct ParticleEmitterComponent // user can change value in editor
    {
        bool looping;

        Lifetime emitter;

        ParticleEffectShape shape;
        float angle{ 0.f }; // cone shape
        ParticleSimulationSpace space;

        std::chrono::milliseconds emitPeriod;
        std::optional<Burst> burst;

        std::chrono::milliseconds startDelay;
        float startSpeed;

        // each particle
        std::chrono::milliseconds particleLifetime;

        int maxParticles;
    };

    struct ParticleRuntimeComponent // Values that change during play 
    {
        bool active{ false };

        std::chrono::milliseconds adder;
        std::chrono::steady_clock::time_point latestTP;

        int activeCnt = 0;

        UUID target = 0;
    };

    struct ParticleBufferComponent // Actual Particles data for simulation
    {
        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> velocities;
        std::vector<Lifetime> lifetimes;
    };

    struct ParticleRenderComponent
    {
        AssetID texture;
        float size;
        glm::vec4 color;
    };

    struct ParticleData
    {
        ParticleEmitterComponent& emitter;
        ParticleRuntimeComponent& runtime;
        ParticleBufferComponent& buffer;
        ParticleRenderComponent& render;
    };

    struct ParticleAttachmentComponent
    {
        UUID target{ 0 };
        AssetID particle;
    };
}

#endif //MANGO_PARTICLE_H