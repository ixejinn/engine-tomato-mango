#ifndef MANGO_TRANSFORMSYSTEM_H
#define MANGO_TRANSFORMSYSTEM_H

#include "ECS/Systems/System.h"

namespace tomato {
    class TransformSystem : public System {
    public:
        void Update(SimContext &simCtx) override;

    private:
        void UpdateScreenUI(SimContext& simCtx);
    };
}

#endif //MANGO_TRANSFORMSYSTEM_H