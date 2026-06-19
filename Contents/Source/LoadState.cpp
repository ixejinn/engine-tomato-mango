#include "LoadState.h"
#include "Resource/AssetRegistry.h"
#include "Resource/Render/Texture.h"
#include "Serialization/ComponentSerializer.h"

using namespace tomato;

void LoadState::Init()
{
	Texture::Create("Resources/Contents/WATER_GAME_LOGO.png");
	Serialization::LoadScene(registry_, "Resources/Engine/Assets/test.data");

}

void LoadState::Update() {}

void LoadState::Exit() {}
