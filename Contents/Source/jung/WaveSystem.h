#ifndef MANGO_WAVESYSTEM_H
#define MANGO_WAVESYSTEM_H

#include "ECS/Systems/System.h"

class WaveSystem : public tomato::System
{
public:
    void Update(tomato::SimContext& simCtx);
};

#endif //MANGO_WAVESYSTEM_H