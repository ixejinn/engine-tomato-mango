#ifndef MANGO_FRICTIONSYSTEM_H
#define MANGO_FRICTIONSYSTEM_H

#include "ECS/Systems/System.h"

namespace tomato
{
    class FrictionSystem : public System
    {
    public:
        void Update(SimContext& simCtx);
    };
}

#endif //MANGO_FRICTIONSYSTEM_H
