#ifndef MANGO_BITMASKFWD_H
#define MANGO_BITMASKFWD_H

#include <type_traits>
#include "Input/InputConstantsFwd.h"
#include "Collision/CollisionConstantsFwd.h"
#include "ECS/Forward/SystemFrameworkFwd.h"

namespace tomato {
    template<typename T>
    struct is_bitmask : std::false_type {};

    template<> struct is_bitmask<InputIntent>       : std::true_type {};
    template<> struct is_bitmask<CollisionLayer>    : std::true_type {};
    template<> struct is_bitmask<ColliderType>      : std::true_type {};
    template<> struct is_bitmask<RunMode>           : std::true_type {};
}

#endif //MANGO_BITMASKFWD_H