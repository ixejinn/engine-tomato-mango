#include <fstream>
#include <entt/entt.hpp>

#include "Serialization/SerializeManager.h"
#include "Serialization/JsonIO.h"

#include "State/State.h"
#include "State/StateRegistry.h"

#include "ECS/Components/Nametag.h"
#include "ECS/Entity/Entity.h"

#include "Utils/Logger.h"

namespace tomato::Serialization
{
	void SerializeManager::SaveState(State* state, const char* path)
	{
		json root;
		auto& reg = state->GetRegistry();

		root["State"] = StateRegistry::GetInstance().GetStateID(typeid(*state));
		root["State Name"] = std::type_index(typeid(*state)).name();

		resource_.SaveResources(root);
		particle_.SaveAttachments(root, reg);

		root["Entities"] = json::array();

		auto view = reg.view<NametagComponent>();
		for (auto entity : view)
		{
			json entityJson;

			state_.SaveEntity(entityJson, reg, entity);
			if (!entityJson.empty())
				root["Entities"].push_back(entityJson);
		}

		std::ofstream ofs(path);
		ofs << root.dump(4);
	}

	void SerializeManager::LoadState(State* state, const char* path)
	{
		json root = LoadJsonData(path);
		if (root == nullptr)
			return;

		AssetID stateID = root["State"];

//		auto newState = state->GetStateByID(stateID);
//		if (!newState)
//		{
//			TMT_ERR << "Invalid State ID in " << path;
//			return;
//		}
//
//		auto& registry = newState->GetRegistry();
//
//		//Load Resources
//		resource_.LoadResources(root);
//		state_.CreateEntities(root, registry, newState->GetEntityMap());
//		state_.LoadComponents(root, registry, newState->GetEntityMap());
//
//		//ResolveHierarchy(registry, newState->GetEntityMap());
//
//		particle_.Attach(root, registry);
//
//		state->SetNextState(std::move(newState));
//		registry.ctx().get<EntityNameGenerator>().Initialize(registry);
	}

	void SerializeManager::NewState(State* state)
	{
//		AssetID stateID = StateRegistry::GetInstance().GetStateID(typeid(*state));
//		auto newState = state->GetStateByID(stateID);
//		state->SetNextState(std::move(newState));
	}
}