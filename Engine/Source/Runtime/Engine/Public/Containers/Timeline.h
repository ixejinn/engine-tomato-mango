#ifndef MANGO_TIMELINE_H
#define MANGO_TIMELINE_H

#include <cstddef>
#include <cstdint>
#include "RingArray.h"
#include "Network/NetworkConfig.h"

namespace tomato {
    /**
     * @brief RingArray that stores values of type T indexed by simulation tick.
     * @tparam T Type of elements.
     * @tparam N Window size. Must be a power of 2.
     *
     * 시뮬레이션 틱을 인덱스로 사용해 값을 저장 및 조회하는 고정 크기 RingArray.
     * 롤백에 사용하기 위해 최근 N틱 동안의 T 값을 덮어쓰며 저장하는 타임라인 윈도우.
     */
    template<typename T, std::size_t N = ROLLBACK_WINDOW>
    class Timeline
    {
    public:
        T& operator[](uint32_t tick) { return data_[tick]; }
        const T& operator[](uint32_t tick) const { return data_[tick]; }

        void SetData(uint32_t tick, const T& newData) { data_[tick] = newData; }
        void SetData(uint32_t tick, T&& newData) { data_[tick] = std::move(newData); }

        template<typename... Args>
        void EmplaceData(uint32_t tick, Args&&... args) { data_[tick] = T(std::forward<Args>(args)...); }

    private:
        RingArray<T, N> data_;
    };
}

#endif //MANGO_TIMELINE_H