#include <entt/entt.hpp>
#include "ECS/Systems/CameraTransformSystem.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Camera.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "Services/Window.h"
#include "Utils/Logger.h"

namespace tomato
{
    void CameraTransformSystem::Update(SimContext& simCtx)
    {
        auto& registry = simCtx.state->GetRegistry();

        auto view = registry.view<TransformComponent, CameraComponent>();
        for (auto [e, trf, cam] : view.each()) {
            if (!cam.dirty)
                continue;

            auto quaternion = trf.GetWorldQuaternion();
            cam.back = quaternion * glm::vec3(0, 0, 1);
            cam.right = quaternion * glm::vec3(1, 0, 0);
            cam.up = quaternion * glm::vec3(0, 1, 0);

            auto pos = trf.GetWorldPosition();
            cam.view = glm::mat4
            {
                cam.right.x, cam.up.x, cam.back.x, 0,   // column 0
                cam.right.y, cam.up.y, cam.back.y, 0,   // column 1
                cam.right.z, cam.up.z, cam.back.z, 0,   // column 2
                -glm::dot(cam.right, pos), -glm::dot(cam.up, pos), -glm::dot(cam.back, pos), 1
            };

            const float width = static_cast<float>(Window::GetWidth());
            const float height = static_cast<float>(Window::GetHeight());

            switch (cam.mode)
            {
            case Perspective:
                cam.projection = glm::perspective(
                    glm::radians(cam.degree),
                    width / height,
                    cam.zNear, cam.zFar);
            break;

            case Orthogonal:
            {
                float w = width / height * 10;
                cam.projection = glm::ortho(
                    -w, w,
                    -10.f, 10.f,
                    cam.zNear, cam.zFar);
            }
            break;
            }

            cam.viewProjMat = cam.projection * cam.view;

            cam.dirty = false;
        }
    }
}