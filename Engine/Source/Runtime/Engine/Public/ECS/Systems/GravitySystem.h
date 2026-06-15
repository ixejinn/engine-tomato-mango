#ifndef MANGO_GRAVITYSYSTEM_H
#define MANGO_GRAVITYSYSTEM_H

#include "ECS/Systems/System.h"

namespace tomato {
    class GravitySystem : public System {
    public:
        void Update(SimContext& simCtx);

    private:
        constexpr static float GRAVITY{-0.5f};
    };
}

#endif //MANGO_GRAVITYSYSTEM_H
