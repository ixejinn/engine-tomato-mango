#ifndef MANGO_ENUM_CLASS_FLAGS_MACRO_H
#define MANGO_ENUM_CLASS_FLAGS_MACRO_H

#include <type_traits>
#define TMT_ENABLE_ENUM_FLAGS(Enum)                                      \
constexpr Enum operator|(Enum a, Enum b)                                 \
{                                                                         \
    return static_cast<Enum>(                                             \
        static_cast<std::underlying_type_t<Enum>>(a) |                    \
        static_cast<std::underlying_type_t<Enum>>(b));                    \
}                                                                         \
                                                                          \
constexpr Enum operator&(Enum a, Enum b)                                 \
{                                                                         \
    return static_cast<Enum>(                                             \
        static_cast<std::underlying_type_t<Enum>>(a) &                    \
        static_cast<std::underlying_type_t<Enum>>(b));                    \
}                                                                         \
                                                                          \
constexpr Enum& operator|=(Enum& a, Enum b)                              \
{                                                                         \
    return a = a | b;                                                     \
}

namespace tomato
{
    template<typename Enum>
    constexpr bool HasFlag(Enum value, Enum flag)
    {
        using T = std::underlying_type_t<Enum>;
        return (static_cast<T>(value) & static_cast<T>(flag)) != 0;
    }
}

#endif // !MANGO_ENUM_CLASS_FLAGS_MACRO_H
