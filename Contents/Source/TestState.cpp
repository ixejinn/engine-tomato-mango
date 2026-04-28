#include "TestState.h"
#include "Resource/AssetRegistry.h"
#include "Resource/Audio/Audio.h"
#include "Utils/Logger.h"
using namespace tomato;

void TestState::Init() {
    TMT_INFO << "Test State";
    auto id = Audio::Create("Resources/Contents/filename");
    auto audioPtr = AssetRegistry<Audio>::GetInstance().Get(id);
    audioPtr->Start();
}

void TestState::Exit() {
}
