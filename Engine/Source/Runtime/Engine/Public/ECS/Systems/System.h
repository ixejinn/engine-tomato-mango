#ifndef MANGO_SYSTEM_H
#define MANGO_SYSTEM_H

#include "ECS/Forward/SystemFwd.h"

namespace tomato {
    class System {
    public:
        virtual ~System() = default;

        virtual void Update(SimContext& simCtx) = 0;
    };
}

#endif //MANGO_SYSTEM_H