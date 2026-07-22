#include "LoadState.h"
#include "Resource/AssetRegistry.h"
#include "Resource/Render/Texture.h"
#include "ECS/Components/Nametag.h"
#include "Serialization/ComponentSerializer.h"

#include "Engine.h"
#include "Prefab/Prefab.h"
#include "Prefab/UIPrefab.h"
#include "TestState.h"
#include "Utils/RegistryEntry.h"
REGISTER_STATE(LoadState)

using namespace tomato;

void LoadState::Init()
{
	Texture::Create("Resources/Contents/WATER_GAME_LOGO.png");
	/*Texture::Create("Resources\\Contents\\WATER_GAME_LOGO.png");*/
	//Serialization::LoadScene(registry_, "Resources/Engine/Scenes/default.scene", entityMap_);
	
	//entt::entity btn;// = entityMap_[11560345711817135869];
	//auto btnView = registry_.view<NametagComponent>();
	//for (auto [e, tag] : btnView.each())
	//{
	//	if (tag.name == "Button")
	//		btn = e;
	//}

	//auto& mouseEvt = registry_.get<MouseEventComponent>(btn);
	//mouseEvt.onClick =
	//	[this](const MouseClickEvent& e)
	//	{
	//		uiController_.onClick(e);
	//	};
	//Prefab::CreateCamera(registry_);
	//auto btn = UIPrefab::CreateButton(registry_);
	//auto& mouseEvt = registry_.get<MouseEventComponent>(btn);
	//mouseEvt.onClick =
	//	[this](const MouseClickEvent& e)
	//	{
	//		engine_.RequestMatchToServer();
	//		uiController_.onMatchRequest(e);
	//	};
}

void LoadState::Update()
{
	//engine_.TryStartGame(std::make_unique<TestState>(engine_));
}

void LoadState::Exit() {}
