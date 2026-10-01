#ifndef MANGO_RENDERSORTKEY_H
#define MANGO_RENDERSORTKEY_H

#include <cstdint>
#include <algorithm>

namespace tomato
{
    enum class RenderPriority : uint8_t {
        Background  = 10,
        Opaque      = 20,
        Skybox      = 30,
        Transparent = 40,
        Overlay     = 50
    };

    ///    64 bits    ________ ________ ________ ________ ________ ________ ________ ________
    /// Opaque      : priority|      shader     |      texture    |      mesh       |
    /// Transparent : priority|              ~distSq              |     shader      |

    inline uint64_t GetOpaqueSortKey(RenderPriority p, uint16_t shader, uint16_t tex, uint16_t mesh)
    {
        return (static_cast<uint64_t>(p) << 56) | (static_cast<uint64_t>(shader) << 40) | (static_cast<uint64_t>(tex) << 24) | (static_cast<uint64_t>(mesh) << 8);
    }

    inline uint64_t GetTransparentSortKey(RenderPriority p, float distSq, uint16_t shader)
    {
        uint32_t distSqBits = static_cast<uint32_t>(std::max(distSq, 0.f));
        // 렌더 정렬은 오름차순이므로 distSq의 비트를 뒤집어 거리가 멀수록 앞 순서가 되도록
        return (static_cast<uint64_t>(p) << 56) | (static_cast<uint64_t>(~distSqBits) << 24) | (static_cast<uint64_t>(shader) << 8);
    }
}

#endif //MANGO_RENDERSORTKEY_H