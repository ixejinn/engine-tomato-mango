#ifndef MANGO_COLLISIONFWD_H
#define MANGO_COLLISIONFWD_H

#include <iostream>
#include <entt/entt.hpp>
#include "Containers/UnorderedPair.h"

namespace tomato
{
    using ContactPair = UnorderedPair<entt::entity>;
    inline std::ostream& operator<<(std::ostream& os, const ContactPair& pair)
    {
        os << (int)pair.a << " " << (int)pair.b;
        return os;
    }

    class BroadPhase;
    class NarrowPhase;
}

#endif //MANGO_COLLISIONFWD_H