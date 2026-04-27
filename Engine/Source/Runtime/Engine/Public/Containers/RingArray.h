#ifndef MANGO_RINGARRAY_H
#define MANGO_RINGARRAY_H

#include <array>
#include <cstddef>
#include <bit>

namespace tomato
{
    template <typename T, std::size_t N>
    class RingArray
    {
    public:
        // Type definitions for STL compatibility
        using value_type = T;
        using size_type = std::size_t;
        using reference = T&;
        using const_reference = const T&;
        using pointer = T*;
        using const_pointer = const T*;

        static_assert(N >= 2, "RingArray size must be at least 2");
        static_assert(std::has_single_bit(N), "Size of the RingArray must be power of 2");

        constexpr RingArray() : buffer_{} {} // Zero - initialized(or default - initialized) on construction
        ~RingArray() = default; // std::array automatically manages destruction of T

        // Automatic index wrapping
        constexpr reference operator[](size_type index) noexcept { return buffer_[index & mask]; }
        constexpr const_reference operator[](size_type index) const noexcept { return buffer_[index & mask]; }

        static constexpr size_type Size() noexcept { return N; }
        static constexpr size_type ByteSize() noexcept { return N * sizeof(T); }

    private:
        static constexpr size_type mask = N - 1;
        std::array<T, N> buffer_;
    };
}

#endif //MANGO_RINGARRAY_H