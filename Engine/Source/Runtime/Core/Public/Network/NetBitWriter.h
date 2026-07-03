#ifndef TOMATO_NETBITWRITER_H
#define TOMATO_NETBITWRITER_H

#include <cstdint>
#include <concepts>
#include "Network/NetTypes.h"

namespace tomato
{
    /**
     * @brief Bit-level writer for network message serialization.
     *
     * buffer_에 최소 비트만 사용하여 비트 단위 직렬화한다.
     *
     * @note 현재는 int 계열 기본 타입에 대한 직렬화만 지원하지만,
     *       향후 메시지 타입이 늘어날 경우, 기본 타입에 대한 비트 단위 처리는 BitWriter로 옮길 수 있다.
     */
    class NetBitWriter
    {
    public:
        /**
         * @param out Pointer of raw byte buffer to be sent to the network.
         * @param byteSize Size of the valid data in bytes. Must not exceed MAX_PACKET_SIZE
         */
        NetBitWriter(uint8_t* out, int16_t byteSize);

        /**
         * @param out Pointer of raw byte buffer to be sent to the network.
         */
        explicit NetBitWriter(RawBuffer* out)
            : buffer_(out->data()), byteNum_(MAX_PACKET_SIZE), bitPos_(0) {
        }

        /**
         * @brief Writes an integer value with the range [0, maxValue) to buffer.
         * @tparam T Unsigned integer (uint8_t/uint16_t/uint32_t)
         * @param inValue Input variable to encode.
         * @param maxValue Exclusive upper bound of the value range.
         *
         * [0, maxValue) 범위 내의 inValue 직렬화를 수행한다.
         *
         * @note inValue \< maxValue
         */
        template<typename T>
        requires std::integral<T>
        void WriteInt(const T inValue, int64_t maxValue)
        {
            SerializeInt(inValue, maxValue);
        }

        int16_t GetByteSize() const {
            if (bitPos_ < 0)
                return 0;
            return (bitPos_ + 7) / 8;
        }

    private:
        void SerializeInt(int64_t value, int64_t maxValue);

        uint8_t* buffer_{ nullptr };
        int16_t byteNum_;
        int16_t bitPos_{ -1 };
    };
}

#endif //TOMATO_NETBITWRITER_H