#ifndef MANGO_PARTICLESERIALIZER_H
#define MANGO_PARTICLESERIALIZER_H

#include <entt/fwd.hpp>
#include "Serialization/Json.h"

namespace tomato::Serialization
{
	class ParticleSerializer
	{
	public:
		void SaveAttachments(json& root, entt::registry& reg);
		void Attach(const json& root, entt::registry& reg);
		void SaveParticle(const std::filesystem::path& path, entt::registry& reg, entt::entity entity);
	};
}

#endif // !MANGO_PARTICLESERIALIZER_H