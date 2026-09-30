#ifndef MANGO_NARROWPHASE_H
#define MANGO_NARROWPHASE_H

#include <optional>
#include <entt/fwd.hpp>
#include "Collision/CollisionFwd.h"
#include "Collision/ContactData.h"

namespace tomato
{
    class NarrowPhase
    {
    public:
        virtual ~NarrowPhase() = default;

        virtual std::optional<ContactData> EvaluateContactPair(entt::registry& reg, const ContactPair& pair) = 0;
    };
}

#endif //MANGO_NARROWPHASE_H