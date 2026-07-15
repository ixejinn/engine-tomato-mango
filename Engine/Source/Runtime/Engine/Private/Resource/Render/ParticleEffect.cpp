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
        auto now = std::chrono::steady_clock::now();

        const std::chrono::duration<float> durationFSec(duration_);
        comp.emitter.duration = std::chrono::duration_cast<std::chrono::milliseconds>(durationFSec);
        comp.emitter.start = now;
        comp.looping = looping_;

        comp.shape = shape_;
        comp.angle = angle_;

        const std::chrono::duration<float> emitPeriodFSec(1.f / rateOverTime_);
        comp.emitPeriod = std::chrono::duration_cast<std::chrono::milliseconds>(emitPeriodFSec);
        comp.adder = std::chrono::milliseconds::zero();
        comp.latestTP = now;

        if (burst_.has_value())
        {
            const std::chrono::duration<float> burstPeriodFSec(burst_->period);
            comp.burst->period = std::chrono::duration_cast<std::chrono::milliseconds>(burstPeriodFSec);
            comp.burst->adder = std::chrono::milliseconds::zero();
            comp.burst->latest = now;

            comp.burst->cycles = burst_->cycles;
            comp.burst->count = burst_->count;
        }

        std::chrono::duration<float> startDelayFSec(startDelay_);
        comp.startDelay = std::chrono::duration_cast<std::chrono::milliseconds>(startDelayFSec);
        comp.startSpeed = startSpeed_;

        comp.texture = texture_;
        comp.size = size_;
        comp.color = color_;

        startDelayFSec = std::chrono::duration<float>(lifetime_);
        comp.lifetime = std::chrono::duration_cast<std::chrono::milliseconds>(startDelayFSec);

        comp.positions.clear();
        comp.velocities.clear();
        comp.lifetimes.clear();

        comp.activeCnt = 0;
        comp.maxParticles = maxParticles_;
    }
}