#ifndef MANGO_RENDERTYPE_H
#define MANGO_RENDERTYPE_H

#include <cstdint>

namespace tomato
{
    enum class RenderType : uint16_t
    {
        None = 0,
        // UI,
        Skybox,
        Object
    };
}

#endif //MANGO_RENDERTYPE_H