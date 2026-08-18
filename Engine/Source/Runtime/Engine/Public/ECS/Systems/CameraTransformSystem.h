#ifndef MANGO_CAMERATRANSFORMSYSTEM_H
#define MANGO_CAMERATRANSFORMSYSTEM_H

#include "ECS/Systems/System.h"

namespace tomato
{
    class CameraTransformSystem : public System
    {
    public:
        void Update(SimContext& simCtx) override;
    };
}

#endif //MANGO_CAMERATRANSFORMSYSTEM_H