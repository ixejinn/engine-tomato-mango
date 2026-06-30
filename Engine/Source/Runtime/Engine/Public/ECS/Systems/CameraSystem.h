#ifndef MANGO_CAMERASYSTEM_H
#define MANGO_CAMERASYSTEM_H

#include "ECS/Systems/System.h"

namespace tomato
{
    class CameraSystem : public System
    {
    public:
        void Update(SimContext& simCtx) override;
    };
}

#endif //MANGO_CAMERASYSTEM_H