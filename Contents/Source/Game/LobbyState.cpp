#include "LobbyState.h"
#include "Engine.h"

#include "ECS/Components/Components.h"
#include "ECS/Entity/Hierarchy.h"

#include "Resource/AssetRegistry.h"
#include "Resource/Audio/Audio.h"
#include "Resource/Render/Mesh.h"
#include "Resource/Render/Shader.h"
#include "Resource/Render/Texture.h"
#include "Resource/Render/ParticleEffect.h"
#include "Resource/PathManager.h"

#include "Input/IntentTranslator.h"
#include "Input/InputIntent.h"
#include "Input/KeyConstants.h"

#include "Utils/Logger.h"
#include "Utils/RegistryEntry.h"

#include "Event/EventDispatcher.h"

#include "Prefab/Prefab.h"
#include "Prefab/UIPrefab.h"

#include "GameState.h"

REGISTER_STATE(LobbyState)

using namespace tomato;

LobbyState::LobbyState(Engine& engine) : State(engine) {}

LobbyState::~LobbyState() = default;

void LobbyState::Init()
{
    //Prefab::CreateCamera(registry_, true, true, "Camera");

    auto btn = UIPrefab::CreateButton(registry_, "Match Start", {150.f, 80.f});
    auto& mouseEvt = registry_.get<MouseEventComponent>(btn);

	mouseEvt.onClick =
		[this](const MouseClickEvent& e)
		{
			uiController_.onMatchRequest(e);
			engine_.RequestMatchToServer();
		};
}

void LobbyState::Update()
{
	engine_.TryStartGame(std::make_unique<GameState>(engine_));
}

void LobbyState::Exit()
{
}