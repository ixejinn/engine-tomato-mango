#include "Network/NetBitReader.h"
#include "Network/PacketTypes.h"
#include "Utils/Logger.h"

namespace tomato
{
    NetBitReader::NetBitReader(uint8_t* const in, const int16_t byteSize)
    {
        if (byteSize <= MAX_PACKET_SIZE)
        {
            buffer_ = in;
            byteNum_ = byteSize;
            bitPos_ = 0;
        }
        else
            TMT_ERR << "byteSize exceeds MAX_PACKET_SIZE";
    }

    uint32_t NetBitReader::DeserializeInt(const uint32_t maxValue)
    {
        if (bitPos_ < 0)
        {
            TMT_ERR << "Invalid BitReader";
            return 0;
        }

        // Use local variable to avoid Load-Hit-Store
        uint32_t value = 0;
        int16_t pos = bitPos_;
        int16_t num = byteNum_;

        for (uint32_t mask = 1; (value + mask) < maxValue && mask; mask <<= 1, pos++)
        {
            if (pos >= num * 8)
            {
                TMT_ERR << "Invalid access";
                break;
            }

            if (buffer_[pos >> 3] & (1 << (pos & 7)))
                value |= mask;
        }

        bitPos_ = pos;
        return value;
    }
}