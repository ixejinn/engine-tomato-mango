#ifndef MANGO_PARTICLE_H
#define MANGO_PARTICLE_H

#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "Resource/ResourceFwd.h"

namespace tomato
{
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