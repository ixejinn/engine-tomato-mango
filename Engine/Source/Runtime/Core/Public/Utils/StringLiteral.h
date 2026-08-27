#ifndef MANGO_STRINGLITERAL_H
#define MANGO_STRINGLITERAL_H

#include <cstddef>
#include <algorithm>
#include <string_view>

namespace tomato
{
    template<size_t N>
    struct StringLiteral
    {
        constexpr StringLiteral(char const(&str)[N])
        {
            std::copy_n(str, N, value);
        }

        constexpr bool operator==(const std::string_view other) const noexcept
        {
            return std::string_view(value, N - 1) == other;
        }

        char value[N];
    };
}

#endif //MANGO_STRINGLITERAL_H
