#ifndef MANGO_SAP_H
#define MANGO_SAP_H

#include "Collision/Broad/BroadPhase.h"

namespace tomato {
    class SAP : public BroadPhase {
    public:
        void FindContactPairCandidates(entt::registry &reg, std::vector<ContactPair> &candidates) override;
    };
}

#endif //MANGO_SAP_H