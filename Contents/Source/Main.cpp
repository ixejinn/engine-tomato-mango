#include <iostream>
#include <memory>
#include "Engine.h"
#include "Server/MatchServer.h"
#include "State/StateRegistry.h"
#include "Resource/AssetHash.h"

#include "TestState.h"
#include "LoadState.h"
using namespace tomato;

int main() {
#if 0
     Engine engine(1600, 900, "TOMATO", NetMode::NM_Client);
     engine.SetNextState(std::make_unique<LoadState>(engine));
//    engine.SetNextState(std::make_unique<TestState>(engine));
    /*Engine engine(1600, 900, "TOMATO", NetMode::NM_Alone);
    auto sf = StateRegistry::GetInstance().GetStateFactory(std::type_index(typeid(TestState)));
    engine.SetNextState(sf(engine));*/
    engine.Run();

#elif 1
    auto server = std::make_unique<MatchServer>();
    server->Run();

#endif
}