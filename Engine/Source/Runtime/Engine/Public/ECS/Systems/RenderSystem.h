#ifndef MANGO_RENDERSYSTEM_H
#define MANGO_RENDERSYSTEM_H

#include <entt/fwd.hpp>
#include <vector>
#include <glad/glad.h>
#include "ECS/Systems/System.h"
#include "Render/GLStateFwd.h"

namespace tomato
{
    class RenderSystem : public System
    {
    public:
        RenderSystem();

        void Update(SimContext& simCtx) override;

    private:
        bool UpdateDrawList(SimContext& simCtx);

        struct DrawItem
        {
            uint64_t sortKey;
            entt::entity entity;
        };
        std::vector<DrawItem> drawList_;
    };
}

#endif //MANGO_RENDERSYSTEM_H