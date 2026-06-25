#ifndef MANGO_MEMORYPOOL_H
#define MANGO_MEMORYPOOL_H

#include <cstddef>
#include <cstring>
#include <cstdint>
#include <mutex>
#include "Utils/Logger.h"

namespace tomato
{
    /**
     * @brief Fixed-size memory pool using a free list for fast allocation and deallocation.
     * @tparam T Type of elements.
     * @tparam N Number of elements.
     *
     * N개의 T를 저장할 수 있는 큰 메모리 공간 하나를 할당하고, 메모리 청크 크기만큼 자른다.
     * 메모리 청크의 앞 일부가 다음 메모리 청크의 주소를 가리키는 free-list로 관리한다.
     *
     * @see https://en.wikipedia.org/wiki/Free_list
     */
    template<typename T, std::size_t N = 0>
    class MemoryPool
    {
    public:
        MemoryPool();
        ~MemoryPool();

        MemoryPool(const MemoryPool&) = delete;
        MemoryPool& operator=(const MemoryPool&) = delete;

        MemoryPool(MemoryPool&&) = delete;
        MemoryPool& operator=(MemoryPool&&) = delete;

        /**
         * @brief Allocates a memory block from the pool and constructs an object of type T.
         * @param args Arguments to forward to the constructor of the element.
         * @return Pointer to the constructed object, or nullptr if the pool is exhausted.
         */
        template<typename... Args>
        T* Allocate(Args&&... args);

        /**
         * @brief Destructs an object and returns its memory block to the pool.
         * @param data Pointer to an object previously allocated from this pool.
         * @return true if the object was successfully deallocated, false otherwise.
         */
        bool Deallocate(T* data);

        size_t UsedSize() const { return usedSize_; }
        size_t FreeSize() const { return size_ - usedSize_; }

    private:
        bool IsValid(int idx) const { return valid_[idx >> 3] & (1 << (idx & 7)); }

        void SetValid(int idx)   { valid_[idx >> 3] |=  (1 << (idx & 7)); }
        void SetInvalid(int idx) { valid_[idx >> 3] &= ~(1 << (idx & 7)); }

        void* pool_{nullptr};
        void* free_{nullptr};

        uint8_t* valid_{nullptr};

        std::size_t size_{0};
        std::size_t usedSize_{0};

        std::size_t chunkSize_{0};

        std::mutex mtx_;
    };

    template<typename T, std::size_t N>
    inline MemoryPool<T, N>::MemoryPool()
    : size_(N), chunkSize_(std::max(sizeof(void*), sizeof(T)))
    {
        // 정렬 조건에 맞게 공간 할당
        pool_ = ::operator new(
                chunkSize_ * size_,
                std::align_val_t(std::max(alignof(void*), alignof(T))));

        // 청크들을 링크드 리스트로 연결
        auto base = static_cast<std::byte*>(pool_);
        for (int i = 0; i < size_ - 1; i++)
        {
            void* next = base + (i + 1) * chunkSize_;
            std::memcpy(base + i * chunkSize_, &next, sizeof(void*));
        }
        void* tmp = nullptr;
        std::memcpy(base + (size_ - 1) * chunkSize_, &tmp, sizeof(void*));
        free_ = pool_;

        // 할당 상태 배열 할당 및 초기화
        valid_ = new uint8_t[((size_ - 1) >> 3) + 1]();
    }

    template<typename T, std::size_t N>
    inline MemoryPool<T, N>::~MemoryPool()
    {
        // 반납되지 않은 객체 소멸자 호출
        auto base = static_cast<std::byte*>(pool_);
        for (int i = 0; i < size_; i++)
        {
            if (IsValid(i))
            {
                T* cur = reinterpret_cast<T*>(base + i * chunkSize_);
                cur->~T();
            }
        }

        // 메모리 해제
        ::operator delete(pool_, std::align_val_t(std::max(alignof(void*), alignof(T))));
        delete[] valid_;
    }

    template<typename T, std::size_t N>
    template<typename... Args>
    inline T* MemoryPool<T, N>::Allocate(Args&&... args)
    {
        std::scoped_lock<std::mutex> lock(mtx_);

        if (free_ == nullptr)
        {
            TMT_ERR << "Fulled memory pool - Return nullptr";
            return nullptr;
        }

        usedSize_++;

        void* cur = free_;
        // free_에 *free_(다음 청크의 시작 주소값) 복사
        std::memcpy(&free_, free_, sizeof(void*));

        // 할당 상태 비트 ON
        auto curB = static_cast<std::byte*>(cur);
        auto beginB = static_cast<std::byte*>(pool_);
        auto offset = (curB - beginB) / chunkSize_;
        SetValid(offset);

        return ::new (cur) T(std::forward<Args>(args)...);
    }

    template<typename T, std::size_t N>
    inline bool MemoryPool<T, N>::Deallocate(T* data)
    {
        // 범위 확인
        auto curB = reinterpret_cast<std::byte*>(data);
        auto beginB = static_cast<std::byte*>(pool_);
        auto endB = static_cast<std::byte*>(pool_) + N * chunkSize_;
        if (curB < beginB || curB >= endB)
        {
            TMT_INFO << "Not data in memory pool range";
            return false;
        }

        // 정렬 확인
        auto diffB = curB - beginB;
        if (diffB % chunkSize_ != 0)
        {
            TMT_INFO << "Not the starting address of the chunk of memory pool";
            return false;
        }

        std::scoped_lock<std::mutex> lock(mtx_);

        // 할당 상태 확인
        auto offset = diffB / chunkSize_;
        if (!IsValid(offset))
        {
            TMT_INFO << "Already returned memory chunk";
            return false;
        }

        usedSize_--;

        // 객체 소멸자 호출
        data->~T();

        // *data에 free_(비어있는 청크로 이루어진 링크드 리스트의 head 주소값) 복사
        std::memcpy(data, &free_, sizeof(void*));
        free_ = data;

        // 할당 상태 비트 OFF
        SetInvalid(offset);

        return true;
    }
}

#endif //MANGO_MEMORYPOOL_H