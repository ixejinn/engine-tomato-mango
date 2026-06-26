#include <iostream>
#include <memory>
#include "Engine.h"
#include "TestState.h"
#include "LoadState.h"
using namespace tomato;

int main() {
    Engine engine(1600, 900, "TOMATO", NetMode::NM_Alone);
    engine.SetNextState(std::make_unique<TestState>(engine));
    engine.Run();
}