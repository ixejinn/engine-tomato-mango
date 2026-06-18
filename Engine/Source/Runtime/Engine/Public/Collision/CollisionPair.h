#ifndef MANGO_COLLISIONPAIR_H
#define MANGO_COLLISIONPAIR_H

#include <entt/entt.hpp>
#include "Containers/UnorderedPair.h"

namespace tomato {
    struct CollisionPair {
        CollisionPair(entt::entity a, entt::entity b, bool trigger = false)
            : entities{a, b}, isTrigger(trigger) {}

        UnorderedPair<entt::entity> entities;
        bool isTrigger;

        bool operator==(const CollisionPair& other) const {
            return entities == other.entities;
        }
    };
}

namespace std {
    template<>
    struct hash<tomato::CollisionPair> {
        size_t operator()(const tomato::CollisionPair& p) const {
            uint64_t pack =
                (static_cast<uint64_t>(p.entities.a) << 32) | static_cast<uint64_t>(p.entities.b);
            return std::hash<uint64_t>{}(pack);
        }
    };
}

#endif //MANGO_COLLISIONPAIR_H