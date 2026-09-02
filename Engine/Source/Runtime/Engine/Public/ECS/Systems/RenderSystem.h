#ifndef MANGO_RENDERSYSTEM_H
#define MANGO_RENDERSYSTEM_H

#include <vector>
#include <glm/fwd.hpp>
#include "ECS/Systems/System.h"
#include "ECS/Forward/RenderCompFwd.h"
#include "Resource/ResourceFwd.h"
#include "Render/RenderFwd.h"

namespace tomato
{
    class RenderSystem : public System
    {
    public:
        RenderSystem();

        void Update(SimContext& simCtx) override;

    private:
        bool FrustumCulling(const CameraComponent& cam);

        static uint64_t GetRenderSortKey(const RenderComponent& render, const glm::vec3& pos, const glm::vec3& camPos);

        AssetID curMesh_;
        AssetID curShader_;
        AssetID curTexture_;
    };
}

#endif //MANGO_RENDERSYSTEM_H