#ifndef MANGO_RENDERSYSTEM_H
#define MANGO_RENDERSYSTEM_H

#include <entt/fwd.hpp>
#include <vector>
#include "ECS/Systems/System.h"
#include "Resource/ResourceFwd.h"

namespace tomato
{
    class RenderSystem : public System
    {
    public:
        RenderSystem();

        void Update(SimContext& simCtx) override;

    private:
        void UpdateDrawList(SimContext& simCtx);

        struct DrawItem
        {
            uint64_t sortKey;
            entt::entity entity;
        };
        std::vector<DrawItem> drawList_;

        AssetID curMesh_;
        AssetID curShader_;
        AssetID curTexture_;
    };
}

#endif //MANGO_RENDERSYSTEM_H