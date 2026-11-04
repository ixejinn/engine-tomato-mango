#include <fstream>

#include "Serialization/ParticleSerializer.h"
#include "Serialization/ComponentRegistry.h"

#include "ECS/Components/ActiveTag.h"
#include "ECS/Components/Particle.h"
#include "ECS/Components/Target.h"
#include "ECS/Components/Transform.h"
#include "ECS/Entity/Entity.h"
#include "Particle/ParticleEmitterPool.h"

namespace tomato::Serialization
{
	void ParticleSerializer::SaveAttachments(json& data, entt::registry& reg)
	{
		data["Particle"] = json::array();

		auto view = reg.view<ActiveTag, TransformComponent, ParticleEmitterComponent, TargetComponent>();
		for (auto [e, trf, emitter, target] : view.each())
		{
			if (!emitter.sets.looping)
				continue;

			json particle;
			particle["origin"] = emitter.origin;
			particle["sets"] = emitter.sets;

			particle["position"] = trf.GetLocalPosition();
			particle["rotation"] = trf.GetLocalRotationDegree();

			particle["target"] = GetUUID(reg, target.targetEntity);

			data["Particle"].push_back(particle);
		}
	}

	void ParticleSerializer::Attach(const json& root, entt::registry& reg)
	{
		for (auto& particle : root["Particle"])
		{
			AssetID asset = particle["origin"];
			entt::entity target = GetEntityByUUID(reg, particle["target"]);

			entt::entity ptc = reg.ctx().get<ParticleEmitterPool>().Acquire(reg, target, asset);

			auto& trf = reg.get<TransformComponent>(ptc);
			trf.SetPosition(particle["position"]);
			trf.SetRotationDegree(particle["rotation"]);

			auto& sets = reg.get<ParticleEmitterComponent>(ptc).sets;
			sets = particle["sets"];
		}
	}

	void ParticleSerializer::SaveParticle(const std::filesystem::path& path, entt::registry& reg, entt::entity entity)
	{
		json root = reg.get<ParticleEmitterComponent>(entity).sets;

		std::ofstream ofs(path);
		ofs << root.dump(4);
	}
}