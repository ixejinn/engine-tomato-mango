#ifndef MANGO_PENDINGDESTROY_H
#define MANGO_PENDINGDESTROY_H

#include <cstdint>

namespace tomato
{
    struct PendingDestroyComponent
    {
        uint64_t destroyed;
    };
}

#endif //MANGO_PENDINGDESTROY_H
