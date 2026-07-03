#ifndef MANGO_NETBITREADER_H
#define MANGO_NETBITREADER_H

#include <cstdint>
#include <concepts>

namespace tomato
{
    /**
     * @brief Bit-level reader for network message deserialization.
     *
     * buffer_���� ��Ʈ ������ �ּ� ��Ʈ�� �о� ������ȭ�Ѵ�.
     *
     * @note ����� int �迭 �⺻ Ÿ�Կ� ���� ������ȭ�� ����������,
     *       ���� �޽��� Ÿ���� �þ ���, �⺻ Ÿ�Կ� ���� ��Ʈ ���� ó���� BitReader�� �ű� �� �ִ�.
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
         * [0, maxValue)�� ��Ÿ���� ���� �ʿ��� �ּ� ��Ʈ�� �о� ������ȭ�� �����Ѵ�.
         *
         * @note outValue \< maxValue
         */
        template<typename T>
            requires std::integral<T>
            void ReadInt(T& outValue, int64_t maxValue)
        {
            outValue = static_cast<T>(DeserializeInt(maxValue));
        }

    private:
        int64_t DeserializeInt(int64_t maxValue);

        uint8_t* buffer_{ nullptr };
        int16_t byteNum_;
        int16_t bitPos_{ -1 };
    };
}

#endif //MANGO_NETBITREADER_H
