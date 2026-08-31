#include <memory>
#include "Engine.h"
#include "EngineConfig.h"
#ifdef TOMATO_SERVER
#include "Server/MatchServer.h"
#else
#include "Resource/PathManager.h"
#include "TestState.h"
#include "LoadState.h"
#include "jung/MyState.h"
#endif

// #define TOMATO_SERVER
//#define TOMATO_GREENTEA

using namespace tomato;

int main() {
#if defined(TOMATO_SERVER)
    auto server = std::make_unique<MatchServer>();
    server->Run();

#else //TOMATO_SERVER
    PathManager::SetProjectRoot(TMT_PROJECT_ROOT);
    Engine engine(1600, 900, "TOMATO", NetMode::NM_Client);

#if defined(TOMATO_GREENTEA)
    engine.SetNextState(std::make_unique<TestState>(engine));

#else //TOMATO_GREENTEA
    ////
    engine.SetNextState(std::make_unique<LoadState>(engine));

#endif //TOMATO_GREENTEA
    engine.Run();

#endif //TOMATO_SERVER


   //  // Engine engine(1600, 900, "TOMATO", NetMode::NM_Client);
   //  // engine.SetNextState(std::make_unique<LoadState>(engine));
   //  PathManager::SetProjectRoot(TMT_PROJECT_ROOT);/*
   // std::cout << PathManager::ToProject("Resources\\Contents\\Img\\apple.jpg") << '\n';
   // std::cout << PathManager::ToRuntime("C:\\yj\\engine-tomato-mango\\Contents\\Resources\\Img\\apple.jpg") << '\n';*/
   //
   //  Engine engine(1600, 900, "TOMATO", NetMode::NM_Alone);
   //  engine.SetNextState(std::make_unique<MyState>(engine));
   //
   //  // auto sf = StateRegistry::GetInstance().GetStateFactory(std::type_index(typeid(TestState)));
   //  // engine.SetNextState(sf(engine));
   //  engine.Run();
}