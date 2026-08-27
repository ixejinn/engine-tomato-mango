#ifndef MANGO_STRINGLITERAL_H
#define MANGO_STRINGLITERAL_H

#include <cstddef>
#include <algorithm>

namespace tomato
{
    template<size_t N>
    struct StringLiteral
    {
        constexpr StringLiteral(char const(&str)[N])
        {
            std::copy_n(str, N, value);
        }

        constexpr operator std::string_view() const noexcept
        {
            return { value, N - 1 };
        }

        constexpr bool operator==(std::string_view other) const noexcept
        {
            return std::string_view(value, N - 1) == other;
        }

        char value[N];
    };
}

#endif //MANGO_STRINGLITERAL_H
