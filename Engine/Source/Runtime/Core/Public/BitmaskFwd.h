#ifndef MANGO_BITMASKFWD_H
#define MANGO_BITMASKFWD_H

#include <cstdint>
#include <type_traits>

namespace tomato {
    enum class InputIntent : uint32_t;
    enum class CollisionLayer : uint32_t;
    enum class ColliderType : uint8_t;

    template<typename T>
    struct is_bitmask : std::false_type {};

    template<> struct is_bitmask<InputIntent>    : std::true_type {};
    template<> struct is_bitmask<CollisionLayer> : std::true_type {};
    template<> struct is_bitmask<ColliderType>   : std::true_type {};
}

#endif //MANGO_BITMASKFWD_H