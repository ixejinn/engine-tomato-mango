#ifndef MANGO_PLAYERCAMERASYSTEM_H
#define MANGO_PLAYERCAMERASYSTEM_H

#include "ECS/Systems/System.h"

namespace tomato
{
    class PlayerCameraSystem : public System
    {
    public:
        void Update(SimContext& simCtx) override;
    };
}

#endif //MANGO_PLAYERCAMERASYSTEM_H
