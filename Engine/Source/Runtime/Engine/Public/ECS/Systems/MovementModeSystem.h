#ifndef MANGO_MOVEMENTMODESYSTEM_H
#define MANGO_MOVEMENTMODESYSTEM_H

#include "ECS/Systems/System.h"
#include "ECS/Forward/CharacterCompFwd.h"
#include "ECS/Forward/PhysCompFwd.h"

namespace tomato
{
    class MovementModeSystem : public System
    {
    public:
        void Update(SimContext& simCtx) override;

    private:
        static void StartFalling(MovementComponent& move);
        static void AfterLanding(MovementComponent& move, VelocityComponent& vel);
    };
}

#endif //MANGO_MOVEMENTMODESYSTEM_H
