#include "Network/NetBitWriter.h"
#include "Utils/Logger.h"

namespace tomato
{
    NetBitWriter::NetBitWriter(uint8_t* const out, const int16_t byteSize)
    {
        if (byteSize <= MAX_PACKET_SIZE)
        {
            buffer_ = out;
            byteNum_ = byteSize;
            bitPos_ = 0;
        }
        else
            TMT_ERR << "byteSize exceeds MAX_PACKET_SIZE";
    }

    void NetBitWriter::SerializeInt(const uint32_t value, const uint32_t maxValue)
    {
        if (bitPos_ < 0)
        {
            TMT_ERR << "Invalid BitWriter";
            return;
        }

        // Use local variable to avoid Load-Hit-Store
        uint32_t tmp = 0;
        int16_t pos = bitPos_;
        int16_t num = byteNum_;

        for (uint32_t mask = 1; (tmp + mask) < maxValue && mask; mask <<= 1, pos++)
        {
            if (pos >= num * 8)
            {
                TMT_ERR << "Invalid access";
                break;
            }

            // Use local reference to help alias analysis
            // : avoid redundant load/store for the same byte
            uint8_t& byte = buffer_[pos >> 3];
            uint8_t bit = 1 << (pos & 7);

            if (value & mask)   // set 1
            {
                byte |= bit;
                tmp |= mask;
            }
            else                // set 0
                byte &= ~bit;
        }

        bitPos_ = pos;
    }
}