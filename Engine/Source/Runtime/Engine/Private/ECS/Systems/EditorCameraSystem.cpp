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

        if (!isFreeLooking_ && keyDeviceState.IsKeyPressed(Key::RightMouseButton))
        {
            isFreeLooking_ = true;

            preCursorPos.x = keyDeviceState.GetKeyState(Key::MouseX);
            preCursorPos.y = keyDeviceState.GetKeyState(Key::MouseY);
            initCameraEulerDegree = registry.get<TransformComponent>(renderCtx.editorCam).GetWorldRotationDegree();

            simCtx.state->GetWindow().SetCursorDisable(true);
            std::cout << "(( press " << keyDeviceState.GetKeyState(Key::MouseX) << " " << keyDeviceState.GetKeyState(Key::MouseY) << "\n";
        }

        if (isFreeLooking_)
        {
            if (keyDeviceState.IsKeyReleased(Key::RightMouseButton))
            {
                isFreeLooking_ = false;

                simCtx.state->GetWindow().SetCursorDisable(false);
                std::cout << "(( release " << keyDeviceState.GetKeyState(Key::MouseX) << " " << keyDeviceState.GetKeyState(Key::MouseY) << "\n";
                return;
            }

            glm::vec2 currCursorPos{keyDeviceState.GetKeyState(Key::MouseX), keyDeviceState.GetKeyState(Key::MouseY)};
            glm::vec2 deltaCursorPos = currCursorPos - preCursorPos;
            preCursorPos = currCursorPos;
            constexpr float dist = 50.f;

            auto upAxisRadian = glm::atan(deltaCursorPos.x, dist);
            auto rightAxisRadian = glm::atan(deltaCursorPos.y, dist);
            //std::cout << "   upAxis: " << glm::degrees(upAxisRadian) << "\n";
            //std::cout << "rightAxis: " << glm::degrees(rightAxisRadian) << "\n";

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
            auto len = glm::length(dir);
            if (len > 1)
                dir /= len;

            constexpr float speed = 8.f;
            dir *= speed * FIXED_DELTA_TIME;
            auto& trf = registry.get<TransformComponent>(renderCtx.editorCam);

            auto wQuat = trf.GetWorldQuaternion();
            glm::vec3 b = wQuat * glm::vec3(0, 0, 1);
            glm::vec3 r = wQuat * glm::vec3(1, 0, 0);
            glm::vec3 u = wQuat * glm::vec3(0, 1, 0);

            trf.AddPosition(dir.x * b + dir.y * r);
        }


    }
}
