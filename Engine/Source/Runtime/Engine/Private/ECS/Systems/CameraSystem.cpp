#include <entt/entt.hpp>
#include "ECS/Systems/CameraSystem.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Camera.h"
#include "ECS/SystemUpdateContexts.h"
#include "Services/Window.h"
#include "Utils/Logger.h"
#include "Utils/RegistryEntry.h"
REGISTER_SYSTEM(tomato::SystemPhase::Camera, CameraSystem)

namespace tomato
{
    void CameraSystem::Update(SimContext& simCtx)
    {
        auto& registry = simCtx.state->GetRegistry();
        auto& renderCtx = registry.ctx().get<RenderContext>();

        auto view = registry.view<TransformComponent, CameraComponent>();
        for (auto [e, trf, cam] : view.each()) {
            if (!cam.dirty)
                continue;

            auto quaternion = trf.GetWorldQuaternion();
            glm::vec3 b = quaternion * glm::vec3(0, 0, 1);
            glm::vec3 r = quaternion * glm::vec3(1, 0, 0);
            glm::vec3 u = quaternion * glm::vec3(0, 1, 0);

            auto pos = trf.GetWorldPosition();
            glm::mat4 viewMtx
            {
                r.x, u.x, b.x, 0,   // column 0
                r.y, u.y, b.y, 0,   // column 1
                r.z, u.z, b.z, 0,   // column 2
                -glm::dot(r, pos), -glm::dot(u, pos), -glm::dot(b, pos), 1
            };

            const float width = static_cast<float>(Window::GetWidth());
            const float height = static_cast<float>(Window::GetHeight());

            glm::mat4 projection{1.f};
            switch (cam.mode)
            {
            case Perspective:
                projection = glm::perspective(
                    glm::radians(cam.degree),
                    width / height,
                    cam.zNear, cam.zFar);
            break;

            case Orthogonal:
            {
                float w = width / height * 10;
                projection = glm::ortho(
                    -w, w,
                    -10.f, 10.f,
                    cam.zNear, cam.zFar);
            }
            break;
            }

            cam.viewProjMat = projection * viewMtx;

            cam.dirty = false;
        }

        // Set main camera to render context
        auto viewMainCam = registry.view<MainCameraTag>();
        auto mainCamCnt = viewMainCam.size();
        if (mainCamCnt > 1)
            TMT_WARN << "Invalid main camera count: Expected 1, found " << mainCamCnt;
        else if (mainCamCnt < 1) {
             TMT_WARN << "Invalid main camera count: Expected at least 1 camera";
            renderCtx.mainCam = entt::null;
            return;
        }

        renderCtx.mainCam = viewMainCam.front();
    }
}