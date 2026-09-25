#ifndef MANGO_INTEGRATIONSYSTEM_H
#define MANGO_INTEGRATIONSYSTEM_H

#include "ECS/Systems/System.h"

namespace tomato {
    class IntegrationSystem : public System {
    public:
        void Update(SimContext& simCtx) override;

    private:
        static constexpr float VELOCITY_SQ_EPSILON = 1e-6f;
    };
}

#endif //MANGO_INTEGRATIONSYSTEM_H
