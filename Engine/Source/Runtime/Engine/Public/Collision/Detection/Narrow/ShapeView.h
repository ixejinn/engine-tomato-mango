#ifndef MANGO_SHAPEVIEW_H
#define MANGO_SHAPEVIEW_H

#include "ECS/Forward/PhysCompFwd.h"

namespace tomato
{
    // Narrow phase 알고리즘이 registry를 몰라도 되도록 하기 위함
    struct ShapeView
    {
        const ColliderComponent& col;
        const TransformComponent& trf;
    };
}

#endif //MANGO_SHAPEVIEW_H