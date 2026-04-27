#ifndef MANGO_SPSCQUEUE_H
#define MANGO_SPSCQUEUE_H

#include <array>
#include <atomic>
#include <cstddef>
#include <bit>
#include <new>
#include "Utils/Logger.h"

namespace tomato {
    /**
	* Single Producer Singlce Consumer lock-free queue.
	* Designed to avoid lock contention and frame delay caused by mutex.
	* Only one thread may call Enqueue/Emplace,
	* and only one other thread may call Dequeue.
	*
	* Fixed size ring buffer using power-of-two size.
	* Index is wrapped by bit mask for fast modulo.
	*/

	/**
	* Enqueue vs Emplace
	* Enqueue : create object first, then move into queue.
	* Emplace : create object directly inside queue memory.
	*/


	/**
	* NOTE:
	* 64 bytes is the cache line size on all mainstream x86/ARM CPUs,
	* so we intentionally fix it for ABI stability.
	*/
	inline constexpr std::size_t CACHE_LINE_SIZE = 64;

	template<typename T, std::size_t sizeT>
	class SPSCQueue
	{
	public:
		static_assert(sizeT >= 2, "SPSCQueue size must be at least 2");
		static_assert(std::has_single_bit(sizeT), "Size of the SPSCQueue must be power of 2");

		SPSCQueue() = default;
		~SPSCQueue()
		{
			size_t read = read_.load(std::memory_order_relaxed);
			size_t write = write_.load(std::memory_order_relaxed);

			// Destroy remaining objects in queue.
			// Needed because objects are created with placement new.
			while (read != write)
			{
				auto* item = reinterpret_cast<T*>(&cells_[read & mask_].buffer_);
				item->~T();
				++read;
			}
		}

		/**
		* Push already constructed object into queue.
		* This moves the object into internal buffer.
		*
		* write index uses release,
		* read index uses acquire to quarantee visibility
		* between producer and consumer thread.
		*/
		bool Enqueue(T item)
		{
			const auto write = write_.load(std::memory_order_relaxed);
			auto read = read_.load(std::memory_order_acquire);

			if(write - read == sizeT)
			{
				TMT_ERR << "SPSCQueue is full.";
				//Error -> Disconnect
				return false;
			}

			const auto index = write & mask_;
			new (cells_[index].buffer_) T(std::move(item));

			write_.store(write + 1, std::memory_order_release);
			return true;
		}

		bool Dequeue(T& out)
		{
			auto read = read_.load(std::memory_order_relaxed);
			const auto write = write_.load(std::memory_order_acquire);
			if (read == write)
			{
				TMT_ERR << "SPSCQueue is empty.";
				return false;
			}

			const auto index = read & mask_;
			auto* item = reinterpret_cast<T*>(cells_[index].buffer_);

			out = std::move(*item);
			item->~T();

			read_.store(read + 1, std::memory_order_release);
			return true;
		}

		/**
		* Construct object directly inside queue buffer.
		* Avoids temporary object and extra move.
		*/
		template<typename... Args>
		bool Emplace(Args&&... args)
		{
			const auto write = write_.load(std::memory_order_relaxed);
			auto read = read_.load(std::memory_order_acquire);
			const auto index = write & mask_;

			if (write - read == sizeT)
			{
				TMT_ERR << "SPSCQueue is full.";
				return false;
			}

			new (&cells_[index].buffer_) T(std::forward<Args>(args)...);

			write_.store(write + 1, std::memory_order_release);
			return true;
		}

		const T* Front() const
		{
			auto read = read_.load(std::memory_order_acquire);
			auto write = write_.load(std::memory_order_acquire);

			if (read == write)
				return nullptr;

			const auto index = read & mask_;
			return reinterpret_cast<const T*>(cells_[index].buffer_);
		}

		bool Empty() const { return write_.load(std::memory_order_acquire) == read_.load(std::memory_order_acquire); }
		const std::size_t Capacity() const { return sizeT; }
		const std::size_t Size() const
		{
			const auto write = write_.load(std::memory_order_relaxed);
			auto read = read_.load(std::memory_order_acquire);

			return write - read;
		}

	#ifdef TOMATO_DEBUG
		const T* DebugReadValue() const
		{
			size_t read = read_.load(std::memory_order_relaxed);
			size_t write = write_.load(std::memory_order_relaxed);
			if (read == write) return nullptr;

			return reinterpret_cast<const T*>(cells_[read & mask_].buffer_);
		}
	#endif

	private:
		struct cell_t {
			alignas(T) std::byte buffer_[sizeof(T)];
		};

		// size if power of two, so index wrap can be done with bit mask.
		static constexpr std::size_t mask_ = sizeT - 1;

		// Separate read and write index into different cache lines
		// to avoid false sharing between threads.
		alignas(CACHE_LINE_SIZE) std::atomic<size_t> write_ = 0;
		alignas(CACHE_LINE_SIZE) std::atomic<size_t> read_ = 0;

		alignas(CACHE_LINE_SIZE) std::array<cell_t, sizeT> cells_;
	};
}

#endif //MANGO_SPSCQUEUE_H