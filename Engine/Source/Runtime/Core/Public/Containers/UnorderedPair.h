#ifndef MANGO_UNORDEREDPAIR_H
#define MANGO_UNORDEREDPAIR_H

#include "entt/fwd.hpp"

namespace tomato {
    template<typename T>
    struct UnorderedPair {
        UnorderedPair() = default;
        UnorderedPair(T x, T y) : a(x), b(y) {
            if (x > y) {
                a = y;
                b = x;
            }
        }

        bool operator==(const UnorderedPair<T>& other) const {
            return a == other.a && b == other.b;
        }

        T a, b;
    };
}

namespace std {
    template<>
    struct hash<tomato::UnorderedPair<entt::entity>> {
        size_t operator()(const tomato::UnorderedPair<entt::entity>& p) const {
            uint64_t pack =
                    (static_cast<uint64_t>(p.a) << 32) | static_cast<uint64_t>(p.b);
            return std::hash<uint64_t>{}(pack);
        }
    };
}

#endif //MANGO_UNORDEREDPAIR_H
