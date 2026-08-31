#ifndef MANGO_TRANSFORMDIRTY_H
#define MANGO_TRANSFORMDIRTY_H

#include <cstdint>

namespace tomato::Transform
{
    enum class Dirty : uint8_t
    {
        None        = 0,
        Local       = 1 << 0,
        Hierarchy   = 1 << 1,
    };
}

#endif //MANGO_TRANSFORMDIRTY_H