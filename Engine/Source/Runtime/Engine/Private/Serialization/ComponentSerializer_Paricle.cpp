#include "Serialization/ComponentSerializer.h"

#include "ECS/Components/Particle.h"

namespace tomato::Serialization
{
	void Save(json& data, entt::registry& registry, const ParticleEmitterComponent& particle)
	{
		data["duration"] = std::chrono::duration<float>(particle.emitter.duration).count();
		data["looping"] = particle.looping;

		data["startDelay"] = std::chrono::duration<float>(particle.startDelay).count();
		data["startSpeed"] = particle.startSpeed;

		data["maxParticles"] = particle.maxParticles;
		data["shape"] = particle.shape;
		data["angle"] = particle.angle;
		data["space"] = particle.space;

		data["lifetime"] = std::chrono::duration<float>(particle.particleLifetime).count();
		data["rateOverTime"] = std::chrono::duration<float>(particle.emitPeriod).count();
		if (particle.burst.has_value())
		{
			data["burst"]["period"] = std::chrono::duration<float>(particle.burst.value().period).count();
			data["burst"]["cycles"] = particle.burst.value().cycles;
			data["burst"]["count"] = particle.burst.value().count;
		}
	}

	void Load(const json& data, entt::registry& registry, ParticleEmitterComponent& particle) {}

	void Save(json& data, entt::registry& registry, const ParticleRenderComponent& particle)
	{
		data["texture"] = particle.texture;
		data["size"] = particle.size;
		data["color"] = {
			particle.color.x,
			particle.color.y,
			particle.color.z,
			particle.color.w
		};
	}

	void Load(const json& data, entt::registry& registry, ParticleRenderComponent& particle) {}
}