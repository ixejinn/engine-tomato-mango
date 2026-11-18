#include "Input/GameInput.h"
#include "Engine/Engine.h"

void SetDefaultGameInput(tomato::Engine& engine, bool use2P)
{
    engine.SetCharacterInputActions(gameCharacterInputActions);
    engine.GetLocalPlayer(0).playerInput.SetMappings(GameInputMappingContext::P1);

    if (use2P)
    {
        if (auto* local2P = engine.AddLocalPlayer(1))
            local2P->playerInput.SetMappings(GameInputMappingContext::P2);
    }
}