#ifndef MANGO_TIMERTESTSYSTEM_H
#define MANGO_TIMERTESTSYSTEM_H

#include "ECS/Systems/System.h"

class TimerTestSystem : public tomato::System
{
public:
    void Update(tomato::SimContext& simCtx);
};

#endif //MANGO_TIMERTESTSYSTEM_H