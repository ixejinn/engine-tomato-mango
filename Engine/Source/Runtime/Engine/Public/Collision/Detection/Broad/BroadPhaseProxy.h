#ifndef MANGO_BROADPHASEPROXY_H
#define MANGO_BROADPHASEPROXY_H

#include "Math/AABB.h"

namespace tomato
{
    struct BroadPhaseProxy
    {
        AABB aabb;
        uint32_t sortKey;   // entity id
        uint32_t entryIndex;
    };

    struct OverlapEntryPair
    {
        uint32_t idxA;
        uint32_t idxB;
    };
}

#endif //MANGO_BROADPHASEPROXY_H