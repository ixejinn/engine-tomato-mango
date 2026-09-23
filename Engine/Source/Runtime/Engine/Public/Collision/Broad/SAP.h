#ifndef MANGO_SAP_H
#define MANGO_SAP_H

#include "Collision/Broad/BroadPhase.h"

namespace tomato
{
    /**
     * @brief Sweep and Prune
     */
    class SAP : public BroadPhase
    {
    public:
        void FindContactPairCandidates(entt::registry& reg, std::vector<ContactPair> &candidates) override;

    private:
        static void UpdateAABBs(entt::registry& reg);
    };
}

#endif //MANGO_SAP_H