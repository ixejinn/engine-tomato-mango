#ifndef MANGO_NARROWPHASE_H
#define MANGO_NARROWPHASE_H

#include <optional>
#include "ECS/PhysCompFwd.h"
#include "Collision/CollisionInfo.h"

namespace tomato {
    class NarrowPhase {
    public:
        virtual ~NarrowPhase() = default;

        virtual std::optional<CollisionInfo> DetectCollision(
            const ColliderComponent& col1, const TransformComponent& trf1,
            const ColliderComponent& col2, const TransformComponent& trf2) = 0;
    };
}

#endif //MANGO_NARROWPHASE_H