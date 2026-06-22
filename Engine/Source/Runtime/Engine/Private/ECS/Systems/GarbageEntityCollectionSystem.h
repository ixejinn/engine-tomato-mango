#ifndef MANGO_GARBAGEENTITYCOLLECTIONSYSTEM_H
#define MANGO_GARBAGEENTITYCOLLECTIONSYSTEM_H

#include "ECS/Systems/System.h"

namespace tomato {
    class GarbageEntityCollectionSystem : public System {
    public:
        void Update(SimContext &simCtx) override;
    };
}

#endif //MANGO_GARBAGEENTITYCOLLECTIONSYSTEM_H