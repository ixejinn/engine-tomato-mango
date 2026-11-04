#include "Particle/ParticleEmitterSettings.h"

namespace tomato
{
    void from_json(const json& j, ParticleBurst& burst)
    {
        burst.time = j.value("time", burst.time);
        burst.count = j.value("count", burst.count);
        burst.cycles = j.value("cycles", burst.cycles);
        burst.interval = j.value("interval", burst.interval);
    }

    void to_json(json& j, const ParticleBurst& burst)
    {
        j["time"] = burst.time;
        j["count"] = burst.count;
        j["cycles"] = burst.cycles;
        j["interval"] = burst.interval;
    }

    void from_json(const json& j, ParticleEmitterSettings& sets)
    {
        sets.duration = j.value("duration", sets.duration);
        sets.looping = j.value("looping", sets.looping);
        sets.startDelay = j.value("startDelay", sets.startDelay);
        sets.maxParticles = j.value("maxParticles", sets.maxParticles);
        sets.simSpace = j.value("simulationSpace", sets.simSpace);

        sets.lifetime = j.value("lifetime", sets.lifetime);
        sets.speed = j.value("speed", sets.speed);
        sets.size = j.value("size", sets.size);
        sets.color = j.value("color", sets.color);

        sets.rateOverTime = j.value("rateOverTime", sets.rateOverTime);
        sets.bursts.clear();
        sets.bursts = j.at("bursts").get<std::vector<ParticleBurst>>();

        sets.shape = j.value("shape", sets.shape);
        sets.coneAngle = j.value("coneAngle", sets.coneAngle);

        sets.texture = j.value("texture", sets.texture);
    }

    void to_json(json& j, const ParticleEmitterSettings& sets)
    {
        j["duration"] = sets.duration;
        j["looping"] = sets.looping;
        j["startDelay"] = sets.startDelay;
        j["maxParticles"] = sets.maxParticles;
        j["simulationSpace"] = sets.simSpace;

        j["lifetime"] = sets.lifetime;
        j["speed"] = sets.speed;
        j["size"] = sets.size;
        j["color"] = sets.color;

        j["rateOverTime"] = sets.rateOverTime;
        j["bursts"] = sets.bursts;

        j["shape"] = sets.shape;
        j["coneAngle"] = sets.coneAngle;

        j["texture"] = sets.texture;
    }
}