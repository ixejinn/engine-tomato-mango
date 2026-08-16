#include <fstream>
#include <string>
#include "Resource/Render/ParticleEffect.h"
#include "Resource/Render/Texture.h"
#include "Resource/AssetRegistry.h"
#include "Serialization/ComponentSerializer.h"
#include "ECS/Components/Particle.h"

namespace tomato
{
    ParticleEffect::ParticleEffect(const std::filesystem::path& path)
    {
        //        const std::string path = "Resource/Contents/";
        //        const std::string file = path + filename;
        auto data = Serialization::LoadJsonData(path.string().c_str());

        duration_ = data["duration"];
        looping_ = data["looping"];

        startDelay_ = data["startDelay"];
        startSpeed_ = data["startSpeed"];

        maxParticles_ = data["maxParticles"];

        shape_ = data["shape"];
        if (shape_ == ParticleEffectShape::Cone)
            angle_ = data["angle"];
        else
            angle_ = 0.f;
        space_ = data["space"];

        lifetime_ = data["lifetime"];

        texture_ = data["texture"];
        size_ = data["size"];
        color_ = { data["color"][0], data["color"][1], data["color"][2], data["color"][3] };

        rateOverTime_ = data["rateOverTime"];

        if (!data["burst"].is_null())
            burst_.emplace(data["burst"]["period"], data["burst"]["cycles"], data["burst"]["count"]);
        else
            burst_ = std::nullopt;

        /*target_ = data["target"];
        offset_ = { data["offset"][0], data["offset"][1], data["offset"][2] };*/
    }

    void ParticleEffect::Create(const std::filesystem::path& path)
    {
        std::unique_ptr<ParticleEffect> ptr{ new ParticleEffect(path) };
        AssetRegistry<ParticleEffect>::GetInstance().Register(path.string(), std::move(ptr));

        TMT_INFO << "Particle Registered " << path;
    }

    void ParticleEffect::InitializeParticleComponent(ParticleData& comp) const
    {
        auto now = std::chrono::steady_clock::now();

        /*Set Particle Emitter Component Values*/
        auto& emitter = comp.emitter;
        const std::chrono::duration<float> durationFSec(duration_);
        emitter.emitter.duration = std::chrono::duration_cast<std::chrono::milliseconds>(durationFSec);
        emitter.emitter.start = now;

        emitter.looping = looping_;

        emitter.shape = shape_;
        emitter.angle = angle_;
        emitter.space = space_;

        if (rateOverTime_ != 0)
        {
            const std::chrono::duration<float> emitPeriodFSec(1.f / rateOverTime_);
            emitter.emitPeriod = std::chrono::duration_cast<std::chrono::milliseconds>(emitPeriodFSec);
        }
        else
            emitter.emitPeriod = std::chrono::milliseconds::zero();

        if (burst_.has_value())
        {
            const std::chrono::duration<float> burstPeriodFSec(burst_->period);
            emitter.burst.emplace(
                    std::chrono::duration_cast<std::chrono::milliseconds>(burstPeriodFSec),
                    std::chrono::milliseconds::zero(),
                    now,
                    burst_->cycles,
                    0,
                    burst_->count);
        }

        std::chrono::duration<float> startDelayFSec(startDelay_);
        emitter.startDelay = std::chrono::duration_cast<std::chrono::milliseconds>(startDelayFSec);
        emitter.startSpeed = startSpeed_;

        startDelayFSec = std::chrono::duration<float>(lifetime_);
        emitter.particleLifetime = std::chrono::duration_cast<std::chrono::milliseconds>(startDelayFSec);

        emitter.maxParticles = maxParticles_;


        /*Set Particle Runtime Component Values*/
        comp.runtime.adder = std::chrono::milliseconds::zero();
        comp.runtime.latestTP = now;
        comp.runtime.activeCnt = 0;


        /*Set Particle Buffer Component Values*/
        comp.buffer.positions.clear();
        comp.buffer.velocities.clear();
        comp.buffer.lifetimes.clear();

        comp.buffer.positions.resize(maxParticles_);
        comp.buffer.velocities.resize(maxParticles_);
        comp.buffer.lifetimes.resize(maxParticles_);


        /*Set Particle Render Component Values*/
        comp.render.texture = texture_;
        comp.render.size = size_;
        comp.render.color = color_;
    }
}