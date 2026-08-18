#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/gtx/string_cast.hpp>
#include "ECS/Components/Transform.h"
#include "ECS/Systems/EditorCameraSystem.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "Simulation/SimulationConfig.h"
#include "State/State.h"
#include "Input/KeyDeviceState.h"
#include "Services/Window.h"

namespace tomato
{
    void EditorCameraSystem::Update(SimContext& simCtx)
    {
        auto& registry = simCtx.state->GetRegistry();
        auto& renderCtx = registry.ctx().get<RenderContext>();
        renderCtx.mainCam = renderCtx.editorCam;

        auto& keyDeviceState = KeyDeviceState::GetInstance();

        //// Start free looking
        if (!isFreeLooking_ && keyDeviceState.IsKeyPressed(Key::RightMouseButton))
        {
            isFreeLooking_ = true;

            preCursorPos.x = keyDeviceState.GetKeyState(Key::MouseX);
            preCursorPos.y = keyDeviceState.GetKeyState(Key::MouseY);
            freeLookStartEulerRad = registry.get<TransformComponent>(renderCtx.editorCam).GetWorldRotationRadian();

            simCtx.state->GetWindow().SetCursorDisable(true);
//            std::cout << "(( press " << keyDeviceState.GetKeyState(Key::MouseX) << " " << keyDeviceState.GetKeyState(Key::MouseY) << "\n";
        }

        if (isFreeLooking_)
        {
            //// Finish free looking
            if (keyDeviceState.IsKeyReleased(Key::RightMouseButton))
            {
                isFreeLooking_ = false;

                simCtx.state->GetWindow().SetCursorDisable(false);
//                std::cout << "(( release " << keyDeviceState.GetKeyState(Key::MouseX) << " " << keyDeviceState.GetKeyState(Key::MouseY) << "\n";
                return;
            }

            //// Update camera position and rotation
            auto& trf = registry.get<TransformComponent>(renderCtx.editorCam);

            // Rotate camera
            glm::vec2 currCursorPos{keyDeviceState.GetKeyState(Key::MouseX), keyDeviceState.GetKeyState(Key::MouseY)};
            glm::vec2 deltaCursorPos = currCursorPos - preCursorPos;
            preCursorPos = currCursorPos;

            const float yawDelta = glm::atan(deltaCursorPos.x, MOUSE_LOOK_SENSITIVITY_DIST);
            const float pitchDelta = glm::atan(deltaCursorPos.y, MOUSE_LOOK_SENSITIVITY_DIST);

            freeLookStartEulerRad.y -= yawDelta;
            freeLookStartEulerRad.x -= pitchDelta;
            freeLookStartEulerRad.x = glm::clamp(freeLookStartEulerRad.x,
                                                 glm::radians(-89.f), glm::radians(89.f));

            trf.SetQuaternion(
                    glm::angleAxis(freeLookStartEulerRad.y, glm::vec3(0, 1, 0)) *
                    glm::angleAxis(freeLookStartEulerRad.x, glm::vec3(1, 0, 0)));

            // Move camera
            int back = 0, right = 0;
            if (keyDeviceState.IsKeyPressed(Key::W))
                --back;
            if (keyDeviceState.IsKeyPressed(Key::S))
                ++back;
            if (keyDeviceState.IsKeyPressed(Key::A))
                --right;
            if (keyDeviceState.IsKeyPressed(Key::D))
                ++right;

            glm::vec2 dir{back, right};
            const float len = glm::length(dir);
            if (len > 1)
                dir /= len;
            dir *= CAMERA_MOVE_SPEED * FIXED_DELTA_TIME;

            const auto wQuat = trf.GetLocalQuaternion();
            const glm::vec3 b = wQuat * glm::vec3(0, 0, 1);
            const glm::vec3 r = wQuat * glm::vec3(1, 0, 0);

            trf.AddPosition(dir.x * b + dir.y * r);
        }


    }
}
