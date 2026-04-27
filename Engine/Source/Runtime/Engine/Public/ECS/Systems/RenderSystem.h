#ifndef MANGO_RENDERSYSTEM_H
#define MANGO_RENDERSYSTEM_H

#include "ECS/Systems/System.h"
#include "Resource/ResourceFwd.h"

namespace tomato {
    class RenderSystem : public System {
    public:
        RenderSystem();

        void Update(SimContext& simCtx) override;

    private:
        AssetID curMesh_;
        AssetID curShader_;
        AssetID curTexture_;
    };
}

#endif //MANGO_RENDERSYSTEM_H