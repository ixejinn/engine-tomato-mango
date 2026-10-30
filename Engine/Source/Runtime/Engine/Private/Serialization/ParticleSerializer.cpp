#include <fstream>

#include "Serialization/ParticleSerializer.h"
#include "Serialization/ComponentRegistry.h"

#include "ECS/Components/Particle.h"
#include "ECS/Entity/Entity.h"
#include "Particle/ParticleEmitterPool.h"

namespace tomato::Serialization
{
	void ParticleSerializer::SaveAttachments(json& data, entt::registry& reg)
	{
		data["Particle"] = json::array();

		auto view = reg.view<ParticleAttachmentComponent>();
		for (auto [e, attach] : view.each())
		{
			json particle;
			particle["particle"] = attach.particle;
			particle["target"] = GetUUID(reg, attach.target);

			data["Particle"].push_back(particle);
		}
	}

	void ParticleSerializer::Attach(const json& root, entt::registry& reg)
	{
		for (auto& particle : root["Particle"])
		{
			AssetID asset = particle["particle"];
			entt::entity target = GetEntityByUUID(reg, particle["target"]);

			reg.ctx().get<ParticleEmitterPool>().Acquire(reg, asset, target);
		}
	}

	void ParticleSerializer::SaveParticle(const std::filesystem::path& path, entt::registry& reg, entt::entity entity)
	{
		json root;

		auto& componentInfo = ComponentRegistry::GetInstance().GetComponentInfo();
		for (auto& info : componentInfo)
		{
			if (info.category == Serialization::ComponentCategory::Particle)
				info.serialization.Save(root, reg, entity);

			/*if (info.name == "Target")
				info.serialization.Save(root, reg, entity);*/
		}

		std::ofstream ofs(path);
		ofs << root.dump(4);
	}
}