#ifndef MANGO_ENUMARRAY_H
#define MANGO_ENUMARRAY_H

#include <array>

namespace tomato
{
    /**
     * @brief Array indexed by enum values.
     * @tparam E Enum for indexing requires E::COUNT.
     * @tparam T Type of elements.
     *
     * E는 0부터 COUNT-1까지 연속.
     */
    template<typename E, typename T>
    class EnumArray
    {
    public:
        EnumArray() : data_() {}

        T& operator[](E enumIdx) { return data_[static_cast<std::size_t>(enumIdx)]; }
        const T& operator[](E enumIdx) const { return data_[static_cast<std::size_t>(enumIdx)]; }

        auto begin() { return data_.begin(); }
        auto end() { return data_.end(); }

    private:
        std::array<T, static_cast<std::size_t>(E::COUNT)> data_;
    };
}

#endif //MANGO_ENUMARRAY_H