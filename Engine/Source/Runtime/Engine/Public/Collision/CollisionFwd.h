#ifndef MANGO_COLLISIONFWD_H
#define MANGO_COLLISIONFWD_H

#include <entt/fwd.hpp>
#include "Containers/UnorderedPair.h"

namespace tomato {
    using CollisionPair = UnorderedPair<entt::entity>;

    class BroadPhase;
    class NarrowPhase;
}

#endif //MANGO_COLLISIONFWD_H