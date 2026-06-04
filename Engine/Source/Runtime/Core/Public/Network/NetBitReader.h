#ifndef MANGO_NETBITREADER_H
#define MANGO_NETBITREADER_H

#include <cstdint>
#include <concepts>

namespace tomato
{
    /**
     * @brief Bit-level reader for network message deserialization.
     *
     * buffer_에서 비트 단위로 최소 비트만 읽어 역직렬화한다.
     *
     * @note 현재는 int 계열 기본 타입에 대한 역직렬화만 지원하지만,
     *       향후 메시지 타입이 늘어날 경우, 기본 타입에 대한 비트 단위 처리는 BitReader로 옮길 수 있다.
     *
     * @see Unreal Engine FBitReader
     */
    class NetBitReader
    {
    public:
        /**
         * @param in Pointer of raw byte buffer received from the network.
         * @param byteSize Size of the valid data in bytes. Must not exceed MAX_PACKET_SIZE
         * @note MAX_PACKET_SIZE is defined in tomato/services/network/CoreNetwork.h
         */
        NetBitReader(uint8_t* in, int16_t byteSize);

        /**
         * @brief Reads an integer value encoded with the range [0, maxValue)
         * @tparam T Unsigned integer (uint8_t/uint16_t/uint32_t)
         * @param outValue Output variable to store the decoded value.
         * @param maxValue Exclusive upper bound of the value range.
         *
         * [0, maxValue)를 나타내기 위해 필요한 최소 비트를 읽어 역직렬화를 수행한다.
         *
         * @note outValue \< maxValue
         */
        template<typename T>
            requires std::same_as<T, uint8_t>
        || std::same_as<T, uint16_t>
            || std::same_as<T, uint32_t>
            void ReadInt(T& outValue, uint32_t maxValue)
        {
            outValue = static_cast<T>(DeserializeInt(maxValue));
        }

    private:
        uint32_t DeserializeInt(uint32_t maxValue);

        uint8_t* buffer_{ nullptr };
        int16_t byteNum_;
        int16_t bitPos_{ -1 };
    };
}

#endif //MANGO_NETBITREADER_H
