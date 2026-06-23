#include "LoadState.h"
#include "Resource/AssetRegistry.h"
#include "Resource/Render/Texture.h"
#include "ECS/Components/Nametag.h"
#include "Serialization/ComponentSerializer.h"

using namespace tomato;

void LoadState::Init()
{
	Texture::Create("Resources/Contents/WATER_GAME_LOGO.png");
	Serialization::LoadScene(registry_, "Resources/Engine/Assets/test.data");

	entt::entity btn;
	auto btnView = registry_.view<NametagComponent>();
	for (auto [e, tag] : btnView.each())
	{
		if (tag.name == "Button")
			btn = e;
	}

	auto& mouseEvt = registry_.get<MouseEventComponent>(btn);
	mouseEvt.onClick =
		[this](const MouseClickEvent& e)
		{
			uiController_.onClick(e);
		};
}

void LoadState::Update() {}

void LoadState::Exit() {}
