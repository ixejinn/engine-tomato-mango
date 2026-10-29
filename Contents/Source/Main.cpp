#include <memory>
#include "Engine/Engine.h"
#include "Engine/EngineConfig.h"

//#define TOMATO_SERVER
#ifdef TOMATO_SERVER
#include "Server/MatchServer.h"

#else
#include "Resource/PathManager.h"
#include "PlayTest/TestState.h"
#include "PlayTest/LoadState.h"
#include "PlayTest/ExampleState.h"
#include "PlayTest/RenderTestState.h"
#include "Game/GameState.h"
#include "Game/LobbyState.h"
#endif

#define TOMATO_GREENTEA

using namespace tomato;

int main() {
#if defined(TOMATO_SERVER)
    auto server = std::make_unique<MatchServer>();
    server->Run();

#else //TOMATO_SERVER
    PathManager::SetProjectRoot(TMT_PROJECT_ROOT);
    Engine engine(1920, 1080, "TOMATO", NetMode::NM_Alone);

#if defined(TOMATO_GREENTEA)
    engine.SetNextState(std::make_unique<ExampleState>(engine));

#else //TOMATO_GREENTEA
    engine.SetNextState(std::make_unique<GameState>(engine));

#endif //TOMATO_GREENTEA
    engine.Run();

#endif //TOMATO_SERVER
}