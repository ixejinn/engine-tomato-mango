#ifndef MANGO_VIEWGIZMORENDERSYSTEM_H
#define MANGO_VIEWGIZMORENDERSYSTEM_H

#include "ECS/Systems/System.h"

namespace tomato
{
    class ViewGizmoRenderSystem : public System
    {
    public:
        void Update(SimContext& simCtx);
    };
}

#endif //MANGO_VIEWGIZMORENDERSYSTEM_H