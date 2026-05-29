#ifndef MANGO_INTEGRATIONSYSTEM_H
#define MANGO_INTEGRATIONSYSTEM_H

#include "ECS/Systems/System.h"

namespace tomato {
    class IntegrationSystem : public System {
    public:
        void Update(SimContext& simCtx) override;
    };
}

#endif //MANGO_INTEGRATIONSYSTEM_H
