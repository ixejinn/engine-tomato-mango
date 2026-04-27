#ifndef MANGO_BITMASKOPERATORS_H
#define MANGO_BITMASKOPERATORS_H

#include <type_traits>
#include <ostream>
#include "BitmaskFwd.h"

namespace tomato
{
    template<typename T>
    concept Bitmask = std::is_enum_v<T> && is_bitmask<T>::value;

    template<Bitmask T>
    inline T operator&(const T l, const T r)
    {
        using U = std::underlying_type_t<T>;
        return static_cast<T>(static_cast<U>(l) & static_cast<U>(r));
    }

    template<Bitmask T>
    inline T operator|(const T l, const T r)
    {
        using U = std::underlying_type_t<T>;
        return static_cast<T>(static_cast<U>(l) | static_cast<U>(r));
    }

    template<Bitmask T>
    inline T operator~(const T x)
    {
        using U = std::underlying_type_t<T>;
        return static_cast<T>(~static_cast<U>(x));
    }

    template<Bitmask T>
    inline T operator^(const T l, const T r)
    {
        using U = std::underlying_type_t<T>;
        return static_cast<T>(static_cast<U>(l) ^ static_cast<U>(r));
    }

    template<Bitmask T>
    inline T& operator|=(T& l, const T r)
    {
        l = l | r;
        return l;
    }

    template<Bitmask T>
    inline T& operator&=(T& l, const T r)
    {
        l = l & r;
        return l;
    }

    template<Bitmask T>
    inline bool HasFlag(const T x, const T flag)
    {
        return static_cast<bool>(x & flag);
    }

    template<Bitmask T>
    inline std::ostream& operator<<(std::ostream& out, const T x)
    {
        using U = std::underlying_type_t<T>;
        out << static_cast<U>(x);
        return out;
    }
}

#endif //MANGO_BITMASKOPERATORS_H