#ifndef MANGO_SERIALIZEMANAGER_H
#define MANGO_SERIALIZEMANAGER_H

#include <unordered_map>
#include <entt/fwd.hpp>

#include "Serialization/Json.h"
#include "Serialization/UUID.h"
#include "Serialization/ResourceSerializer.h"
#include "Serialization/StateSerializer.h"
#include "Serialization/ParticleSerializer.h"

#include "State/StateFwd.h"
#include "ECS/Forward/ComponentFwd.h"
namespace tomato
{
	class Engine;
}

namespace tomato::Serialization
{
	class SerializeManager
	{
	public:
		void SaveState(State* state, const char* path);
		void LoadState(Engine& engine_, const char* path);
		void NewState(Engine& engine_, State* state);

		ResourceSerializer& GetResourceSerializer() { return resource_; }
		StateSerializer& GetStateSerializer() { return state_; }
		ParticleSerializer& GetParticleSerializer() { return particle_; }

	private:
		ResourceSerializer resource_;
		StateSerializer state_;
		ParticleSerializer particle_;
		//EngineSerializer engine_;
	};
}

#endif // !MANGO_SERIALIZEMANAGER_H
