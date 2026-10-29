#include "ECS/Systems/PlayerCameraSystem.h"
#include "ECS/SystemFramework/SystemUpdateContexts.h"
#include "ECS/Components/Camera.h"
#include "Prefab/Prefab.h"

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
        else if (mainCamCnt < 1)
        {
            TMT_WARN << "Create player camera";
            renderCtx.mainCam = Prefab::CreateCamera(registry, true, true);
            return;
        }

        renderCtx.mainCam = viewMainCam.front();
    }
}
