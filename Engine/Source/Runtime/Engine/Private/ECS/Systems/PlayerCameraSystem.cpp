#include "ECS/Systems/PlayerCameraSystem.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "ECS/Components/Camera.h"

namespace tomato
{
    void PlayerCameraSystem::Update(SimContext& simCtx)
    {
        auto& registry = simCtx.state->GetRegistry();
        auto& renderCtx = registry.ctx().get<RenderContext>();

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

        if (viewMainCam.empty())
            renderCtx.mainCam = entt::null;

        renderCtx.mainCam = viewMainCam.front();
    }
}
