#include <iostream>
#include <memory>
#include "Engine.h"
#include "Server/MatchServer.h"
#include "State/StateRegistry.h"
#include "Resource/AssetHash.h"
#include "Resource/PathManager.h"

#include "TestState.h"
#include "LoadState.h"
using namespace tomato;

int main() {
#if 1
     // Engine engine(1600, 900, "TOMATO", NetMode::NM_Client);
     // engine.SetNextState(std::make_unique<LoadState>(engine));
     PathManager::SetProjectRoot(TMT_PROJECT_ROOT);
    std::cout << PathManager::ToProject("Resources\\Contents\\Img\\apple.jpg") << '\n';
    std::cout << PathManager::ToRuntime("C:\\yj\\engine-tomato-mango\\Contents\\Resources\\Img\\apple.jpg") << '\n';

     Engine engine(1600, 900, "TOMATO", NetMode::NM_Alone);
     engine.SetNextState(std::make_unique<TestState>(engine));
     // auto sf = StateRegistry::GetInstance().GetStateFactory(std::type_index(typeid(TestState)));
    // engine.SetNextState(sf(engine));
    engine.Run();

#elif 0
    auto server = std::make_unique<MatchServer>();
    server->Run();

#endif
}