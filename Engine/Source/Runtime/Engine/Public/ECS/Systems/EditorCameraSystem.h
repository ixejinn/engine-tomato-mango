#ifndef MANGO_EDITORCAMERASYSTEM_H
#define MANGO_EDITORCAMERASYSTEM_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "ECS/Systems/System.h"
#include "ECS/SystemFramework/SystemFrameworkEventFwd.h"
#include "Input/InputEventFwd.h"

namespace tomato
{
    class EditorCameraSystem : public System
    {
    public:
        EditorCameraSystem();

        void Update(SimContext& simCtx) override;

    private:
        static constexpr float MOUSE_LOOK_SENSITIVITY_DIST{500.f};
        static constexpr float CAMERA_MOVE_SPEED{8.f};

        static constexpr glm::vec3 INIT_POS{0.f, 1.f, 10.f};
        static constexpr glm::vec3 INIT_RAD{0.f};
        void OnChangeRunMode(const ChangeRunModeEvent& e);

        glm::vec3 camPos{INIT_POS};
        glm::vec3 camRad{INIT_RAD};
        bool resetTrf_{true};

        bool isFreeLooking_{false};
        glm::vec2 preCursorPos;
    };
}

#endif //MANGO_EDITORCAMERASYSTEM_H