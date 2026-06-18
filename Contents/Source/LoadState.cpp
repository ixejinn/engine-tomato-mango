#include "LoadState.h"
#include "Serialization/ComponentSerializer.h"

using namespace tomato;

void LoadState::Init()
{
	Serialization::LoadScene(registry_, "Resources/Engine/Assets/test.data");

}

void LoadState::Update() {}

void LoadState::Exit() {}
