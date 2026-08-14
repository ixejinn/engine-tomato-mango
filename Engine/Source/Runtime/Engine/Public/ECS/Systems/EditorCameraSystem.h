#ifndef MANGO_EDITORCAMERASYSTEM_H
#define MANGO_EDITORCAMERASYSTEM_H

#include "ECS/Systems/System.h"
#include "Input/InputEventFwd.h"

namespace tomato
{
    class EditorCameraSystem : public System
    {
    public:
        void Update(SimContext& simCtx) override;

    private:
        void TestMouseMoveInput(CursorPosEvent& e);
    };
}

#endif //MANGO_EDITORCAMERASYSTEM_H