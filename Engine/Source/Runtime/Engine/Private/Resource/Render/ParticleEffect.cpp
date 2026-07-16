#include <fstream>
#include <string>
#include "Resource/Render/ParticleEffect.h"
#include "Resource/Render/Texture.h"
#include "Resource/AssetRegistry.h"
#include "Serialization/ComponentSerializer.h"
#include "ECS/Components/Particle.h"

namespace tomato
{
    ParticleEffect::ParticleEffect(const char* filename)
    {
//        const std::string path = "Resource/Contents/";
//        const std::string file = path + filename;
        auto data = Serialization::LoadJsonData(filename);

        duration_ = data["duration"];
        looping_ = data["looping"];

        startDelay_ = data["startDelay"];
        startSpeed_ = data["startSpeed"];

        maxParticles_ = data["maxParticles"];

        shape_ = data["shape"];
        if (shape_ == ParticleEffectShape::Cone)
            angle_ = data["angle"];
        space_ = data["space"];

        lifetime_ = data["lifetime"];

        texture_ = data["texture"];
        size_ = data["size"];
        color_ = {data["color"][0], data["color"][1], data["color"][2], data["color"][3]};

        rateOverTime_ = data["rateOverTime"];

        if (!data["burst"].is_null())
            burst_.emplace(data["burst"]["period"], data["burst"]["cycles"], data["burst"]["count"]);
        else
            burst_ = std::nullopt;
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
        comp.space = space_;

        if (rateOverTime_ != 0)
        {
            const std::chrono::duration<float> emitPeriodFSec(1.f / rateOverTime_);
            comp.emitPeriod = std::chrono::duration_cast<std::chrono::milliseconds>(emitPeriodFSec);
        }
        else
            comp.emitPeriod = std::chrono::milliseconds::zero();
        comp.adder = std::chrono::milliseconds::zero();
        comp.latestTP = now;

        if (burst_.has_value())
        {
            const std::chrono::duration<float> burstPeriodFSec(burst_->period);
            comp.burst.emplace(
                    std::chrono::duration_cast<std::chrono::milliseconds>(burstPeriodFSec),
                    std::chrono::milliseconds::zero(),
                    now,
                    burst_->cycles,
                    0,
                    burst_->count);
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

        comp.positions.resize(maxParticles_);
        comp.velocities.resize(maxParticles_);
        comp.lifetimes.resize(maxParticles_);

        comp.activeCnt = 0;
        comp.maxParticles = maxParticles_;
    }
}