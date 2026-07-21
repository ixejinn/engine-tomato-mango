#ifndef MANGO_ENUMARRAY_H
#define MANGO_ENUMARRAY_H

#include <array>
#include <initializer_list>

namespace tomato
{
    /**
     * @brief Array indexed by enum values.
     * @tparam E Enum for indexing. Requires E::COUNT at the end and assumes contiguous values from 0 to COUNT-1.
     * @tparam T Type of elements. Requires default constructor.
     */
    template<typename E, typename T>
    class EnumArray
    {
    public:
        EnumArray() : data_() {}

        EnumArray(std::initializer_list<T> list) : data_()
        {
            const int endIdx = static_cast<std::size_t>(E::COUNT);
            for (auto [i, iter] = std::tuple{0, list.begin()}; i < endIdx && iter != list.end(); ++i, ++iter)
                data_[i] = *iter;
        }

        EnumArray(std::initializer_list<std::pair<E, T>> list) : data_()
        {
            for (auto& p : list)
                data_[static_cast<std::size_t>(p.first)] = p.second;
        }

        T& operator[](E enumIdx) { return data_[static_cast<std::size_t>(enumIdx)]; }
        const T& operator[](E enumIdx) const { return data_[static_cast<std::size_t>(enumIdx)]; }

        auto begin() { return data_.begin(); }
        auto end() { return data_.end(); }

    private:
        std::array<T, static_cast<std::size_t>(E::COUNT)> data_;
    };
}

#endif //MANGO_ENUMARRAY_H