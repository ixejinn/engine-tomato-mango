#ifndef MANGO_SAP_H
#define MANGO_SAP_H

#include "Collision/Detection/Broad/BroadPhaseProxy.h"

namespace tomato
{
    /**
     * @brief Sweep and Prune
     */
    class SAP
    {
    public:
        void FindOverlappingPairs(std::vector<BroadPhaseProxy>& proxies, std::vector<OverlapEntryPair>& out);

    private:
        std::vector<uint32_t> active_;
    };
}

#endif //MANGO_SAP_H