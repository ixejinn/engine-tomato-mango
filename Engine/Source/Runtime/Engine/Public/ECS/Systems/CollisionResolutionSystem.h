#ifndef MANGO_COLLISIONRESOLUTIONSYSTEM_H
#define MANGO_COLLISIONRESOLUTIONSYSTEM_H

#include "ECS/Systems/System.h"

namespace tomato
{
    class CollisionResolutionSystem : public System
    {
    public:
        void Update(SimContext& simCtx) override;
    };
}

#endif //MANGO_COLLISIONRESOLUTIONSYSTEM_H
