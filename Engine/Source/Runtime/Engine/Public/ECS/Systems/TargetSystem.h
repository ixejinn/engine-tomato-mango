#ifndef MANGO_TARGETSYSTEM_H
#define MANGO_TARGETSYSTEM_H

#include "ECS/Systems/System.h"

namespace tomato
{
    class TargetSystem : public System
    {
    public:
        void Update(SimContext& simCtx);
    };
}

#endif //MANGO_TARGETSYSTEM_H