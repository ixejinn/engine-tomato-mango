#include <memory>
#include "Engine.h"
#include "EngineConfig.h"

//#define TOMATO_SERVER
#ifdef TOMATO_SERVER
#include "Server/MatchServer.h"

#else
#include "Resource/PathManager.h"
#include "PlayTest/TestState.h"
#include "Game/GameState.h"
#endif

#define TOMATO_GREENTEA

using namespace tomato;

int main() {
#if defined(TOMATO_SERVER)
    auto server = std::make_unique<MatchServer>();
    server->Run();

#else //TOMATO_SERVER
    PathManager::SetProjectRoot(TMT_PROJECT_ROOT);
    Engine engine(1600, 900, "TOMATO", NetMode::NM_Alone);

#if defined(TOMATO_GREENTEA)
    engine.SetNextState(std::make_unique<TestState>(engine));

#else //TOMATO_GREENTEA
    engine.SetNextState(std::make_unique<GameState>(engine));

#endif //TOMATO_GREENTEA
    engine.Run();

#endif //TOMATO_SERVER
}