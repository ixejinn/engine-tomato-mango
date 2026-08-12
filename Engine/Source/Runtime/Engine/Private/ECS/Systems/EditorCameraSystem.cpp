#include <iostream>
#include "ECS/Systems/EditorCameraSystem.h"
#include "Input/InputEvent.h"

namespace tomato
{
    void EditorCameraSystem::Update(SimContext& simCtx)
    {

    }

    void EditorCameraSystem::TestMouseMoveInput(MouseMoveEvent& e)
    {
        std::cout << e.xPos << " " << e.yPos << "\n";
    }
}
