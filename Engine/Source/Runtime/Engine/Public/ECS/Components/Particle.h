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
        ParticleComponent(int spawnCount, AssetID texID) : spawnCnt(spawnCount), texture(texID) {}

        glm::vec4 color{1.f, 1.f, 1.f, 1.f};
        AssetID texture;
        float scale{1.f};

        int spawnCnt;

    // private:
        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> velocities;
    };
}

#endif //MANGO_PARTICLE_H