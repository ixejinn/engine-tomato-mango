#ifndef MANGO_ENUMARRAY_H
#define MANGO_ENUMARRAY_H

#include <array>
#include <initializer_list>
#include <optional>

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

        EnumArray(std::initializer_list<std::pair<E, T>> list)
        {
            for (auto& p : list)
                data_[static_cast<std::size_t>(p.first)] = p.second;
        }

        T& operator[](E enumIdx)
        {
            auto& opt = data_[static_cast<std::size_t>(enumIdx)];
            if (!opt.has_value())
                opt.emplace();
            return opt.value();
        }
        const T& operator[](E enumIdx) const { return data_[static_cast<std::size_t>(enumIdx)].value(); }

        auto begin() { return data_.begin(); }
        auto end() { return data_.end(); }

    private:
        std::array<std::optional<T>, static_cast<std::size_t>(E::COUNT)> data_{};
    };
}

#endif //MANGO_ENUMARRAY_H