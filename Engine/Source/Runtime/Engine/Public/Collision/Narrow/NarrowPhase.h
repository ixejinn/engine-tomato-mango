#ifndef MANGO_NARROWPHASE_H
#define MANGO_NARROWPHASE_H

#include <optional>
#include <entt/fwd.hpp>
#include "ECS/Forward/PhysCompFwd.h"
#include "Collision/CollisionInfo.h"

namespace tomato
{
    class NarrowPhase
    {
    public:
        virtual ~NarrowPhase() = default;

        virtual std::optional<CollisionInfo> EvaluateCollision(
            entt::registry& reg, entt::entity e1, entt::entity e2) = 0;
    };
}

#endif //MANGO_NARROWPHASE_H