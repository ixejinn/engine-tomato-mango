#ifndef MANGO_EDITORCAMERASYSTEM_H
#define MANGO_EDITORCAMERASYSTEM_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "ECS/Systems/System.h"
#include "Input/InputEventFwd.h"

namespace tomato
{
    class EditorCameraSystem : public System
    {
    public:
        void Update(SimContext& simCtx) override;

    private:
        static constexpr float MOUSE_LOOK_SENSITIVITY_DIST{500.f};
        static constexpr float CAMERA_MOVE_SPEED{8.f};

        glm::vec3 freeLookStartEulerRad;
        bool isFreeLooking_{false};

        glm::vec2 preCursorPos;
    };
}

#endif //MANGO_EDITORCAMERASYSTEM_H