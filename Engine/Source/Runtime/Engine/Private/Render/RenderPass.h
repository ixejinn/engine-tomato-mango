#ifndef MANGO_RENDERPASS_H
#define MANGO_RENDERPASS_H

#include "Render/SortKey.h"

namespace tomato
{
    enum class RenderPass : uint8_t
    {
        Opaque = 0,
        Skybox,
        Transparent
    };

    constexpr RenderPass GetRenderPass(uint64_t sortKey) noexcept
    {
        RenderPriority p = GetPriority(sortKey);
        return static_cast<RenderPass>((p >= RenderPriority::Skybox) + (p >= RenderPriority::Transparent));
    }
}

#endif //MANGO_RENDERPASS_H
