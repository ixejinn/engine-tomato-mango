#ifndef MANGO_PROJECTILESYSTEM_H
#define MANGO_PROJECTILESYSTEM_H

#include "ECS/Systems/System.h"

class ProjectileSystem : public tomato::System
{
public:
    void Update(tomato::SimContext& simCtx);
};

#endif //MANGO_PROJECTILESYSTEM_H