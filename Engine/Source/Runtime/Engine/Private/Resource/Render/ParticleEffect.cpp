#include <fstream>
#include <string>
#include <glm/glm.hpp>
#include "Resource/Render/ParticleEffect.h"
#include "Resource/Render/Texture.h"
#include "Resource/AssetRegistry.h"
#include "Serialization/ComponentSerializer.h"
#include "ECS/Components/Particle.h"

namespace tomato
{
    ParticleEffect::ParticleEffect(const char* filename)
    {
        const std::string path = "Resource/Contents/";
        const std::string file = path + filename;
//        auto data = Serialization::LoadJsonData(file.c_str());

        json data;

        data["duration"] = 5;
        data["looping"] = true;

        data["startDelay"] = 0;
        data["startSpeed"] = 1;

        data["maxParticles"] = 100;

        data["shape"] = ParticleEffectShape::Cone;
        data["angle"] = 45;

        data["lifetime"] = 1;

        data["texture"] = GetAssetID(Texture::PrimitiveName);
        data["size"] = 0.1;
        data["color"] = {1, 1, 1, 1};

        data["rateOverTime"] = 5;

        data["burst"] =
                {
                {"period", 1},
                {"cycles", 5},
                {"count", 5}
                };

        std::ofstream ofs(filename);
        ofs << data.dump(4);

        /////////////////
    }

    void ParticleEffect::Create(const char* filename)
    {
        std::unique_ptr<ParticleEffect> ptr{new ParticleEffect(filename)};
        AssetRegistry<ParticleEffect>::GetInstance().Register(filename, std::move(ptr));
    }

    void ParticleEffect::InitializeParticleComponent(ParticleComponent& comp) const
    {
        std::chrono::duration<float> floatSecs(duration_);
        comp.emitter.duration = std::chrono::duration_cast<std::chrono::milliseconds>(floatSecs);
        comp.emitter.start = std::chrono::steady_clock::now();
        comp.activeCnt = 0;

        comp.looping = looping_;

        comp.positions.clear();
        comp.velocities.clear();
        comp.lifetimes.clear();
    }
}