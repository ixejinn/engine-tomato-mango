#ifndef MANGO_COLLISIONPAIR_H
#define MANGO_COLLISIONPAIR_H

#include <entt/fwd.hpp>

namespace tomato {
    struct CollisionPair {
        CollisionPair(entt::entity x, entt::entity y)
                : e1(x), e2(y) {
            if (x > y) {
                e1 = y;
                e2 = x;
            }
        }

        bool operator<(const CollisionPair& other) const {
            return std::tie(e1, e2) < std::tie(other.e1, other.e2);
        }

        bool operator==(const CollisionPair& other) const {
            return e1 == other.e1 && e2 == other.e2;
        }

        entt::entity e1, e2;
    };
}

namespace std {
    template<>
    struct hash<tomato::CollisionPair> {
        size_t operator()(const tomato::CollisionPair& p) const {
            uint64_t pack =
                    (static_cast<uint64_t>(p.e1) << 32) | static_cast<uint64_t>(p.e2);
            return std::hash<uint64_t>{}(pack);
        }
    };
}

#endif //MANGO_COLLISIONPAIR_H
