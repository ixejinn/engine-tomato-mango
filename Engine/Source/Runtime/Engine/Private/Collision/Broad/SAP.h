#ifndef MANGO_SAP_H
#define MANGO_SAP_H

#include "Collision/BroadPhase.h"

namespace tomato {
    class SAP : public BroadPhase {
    public:
        void FindCollisionCandidates(entt::registry &reg, std::vector<CollisionPair> &candidates) override;
    };
}

#endif //MANGO_SAP_H