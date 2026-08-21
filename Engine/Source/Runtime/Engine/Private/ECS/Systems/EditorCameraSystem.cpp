#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/gtx/string_cast.hpp>
#include "ECS/Components/Transform.h"
#include "ECS/Systems/EditorCameraSystem.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "ECS/SystemFramework/ChangeRunModeEvent.h"
#include "Simulation/SimulationConfig.h"
#include "State/State.h"
#include "Input/KeyDeviceState.h"
#include "Services/Window.h"
#include "Event/EventDispatcher.h"

namespace tomato
{
    EditorCameraSystem::EditorCameraSystem()
    {
        EventDispatcher::GetInstance().Connect<ChangeRunModeEvent, &EditorCameraSystem::OnChangeRunMode>(*this);
    }

    void EditorCameraSystem::Update(SimContext& simCtx)
    {
        auto& registry = simCtx.state->GetRegistry();
        auto& renderCtx = registry.ctx().get<RenderContext>();
        renderCtx.mainCam = renderCtx.editorCam;

        auto& keyDeviceState = KeyDeviceState::GetInstance();

        if (mode_ == None)
        {
            // Reset editor camera transform
            if (resetTrf_)
            {
                resetTrf_ = false;

                camPos = INIT_POS;
                camRad = INIT_RAD;

                auto& trf = registry.get<TransformComponent>(renderCtx.editorCam);
                trf.SetPosition(camPos);
                trf.SetQuaternion(glm::quat(camRad));
            }

            // Start free look
            if (keyDeviceState.IsKeyPressed(Key::RightMouseButton))
                mode_ = FreeLook;

            // Start pan
            if (keyDeviceState.IsKeyPressed(Key::MiddleMouseButton))
                mode_ = Pan;

            if (mode_ != None)
            {
                preCursorPos.x = keyDeviceState.GetKeyState(Key::MouseX);
                preCursorPos.y = keyDeviceState.GetKeyState(Key::MouseY);

                simCtx.state->GetWindow().SetCursorDisable(true);
            }
        }

        switch (mode_)
        {
            case FreeLook:
                UpdateFreeLook(simCtx);
                break;

            case Pan:
                UpdatePan(simCtx);
                break;
        }
    }

    void EditorCameraSystem::OnChangeRunMode(const ChangeRunModeEvent &e)
    {
        if (e.newMode == RunMode::Editor)
            resetTrf_ = true;
    }

    void EditorCameraSystem::UpdateFreeLook(SimContext& simCtx)
    {
        auto& registry = simCtx.state->GetRegistry();
        auto& renderCtx = registry.ctx().get<RenderContext>();

        auto& keyDeviceState = KeyDeviceState::GetInstance();

        //// Finish free looking
        if (keyDeviceState.IsKeyReleased(Key::RightMouseButton))
        {
            mode_ = None;

            simCtx.state->GetWindow().SetCursorDisable(false);
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

        camRad.y -= yawDelta;
        camRad.x -= pitchDelta;
        camRad.x = glm::clamp(camRad.x, glm::radians(-89.f), glm::radians(89.f));

        trf.SetQuaternion(
                glm::angleAxis(camRad.y, glm::vec3(0, 1, 0)) *
                glm::angleAxis(camRad.x, glm::vec3(1, 0, 0)));

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

        glm::vec3 dir{right, 0, back};
        const float len = glm::length(dir);
        if (len > 1)
            dir /= len;
        dir *= CAMERA_MOVE_SPEED * FIXED_DELTA_TIME;

        const auto quat = glm::quat(camRad);
        const glm::vec3 r = quat * glm::vec3(1, 0, 0);
        const glm::vec3 b = quat * glm::vec3(0, 0, 1);

        camPos += (dir.x * r + dir.z * b);
        trf.SetPosition(camPos);
    }

    void EditorCameraSystem::UpdatePan(SimContext& simCtx)
    {
        auto& registry = simCtx.state->GetRegistry();
        auto& renderCtx = registry.ctx().get<RenderContext>();

        auto& keyDeviceState = KeyDeviceState::GetInstance();

        //// Finish pan
        if (keyDeviceState.IsKeyReleased(Key::RightMouseButton))
        {
            mode_ = None;

            simCtx.state->GetWindow().SetCursorDisable(false);
            return;
        }

        //// Update camera position and rotation
        auto& trf = registry.get<TransformComponent>(renderCtx.editorCam);

        // Move camera
        glm::vec2 currCursorPos{keyDeviceState.GetKeyState(Key::MouseX), keyDeviceState.GetKeyState(Key::MouseY)};
        glm::vec2 deltaCursorPos = currCursorPos - preCursorPos;
        preCursorPos = currCursorPos;

        glm::vec3 dir{deltaCursorPos.x, -deltaCursorPos.y, 0};

        if (keyDeviceState.IsKeyPressed(Key::W))
            dir.z -= PAN_KEYBOARD_WEIGHT;
        if (keyDeviceState.IsKeyPressed(Key::S))
            dir.z += PAN_KEYBOARD_WEIGHT;
        if (keyDeviceState.IsKeyPressed(Key::A))
            --dir.x;
        if (keyDeviceState.IsKeyPressed(Key::D))
            ++dir.x;

        const float len = glm::length(dir);
        if (len > 1)
            dir /= len;
        dir *= CAMERA_MOVE_SPEED * FIXED_DELTA_TIME;

        const auto quat = glm::quat(camRad);
        const glm::vec3 r = quat * glm::vec3(1, 0, 0);
        const glm::vec3 u = quat * glm::vec3(0, 1, 0);
        const glm::vec3 b = quat * glm::vec3(0, 0, 1);

        camPos += (dir.x * r + dir.y * u + dir.z * b);
        trf.SetPosition(camPos);
    }
}
