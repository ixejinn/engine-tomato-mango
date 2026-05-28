#ifndef MANGO_INPUTCONSTANTS_H
#define MANGO_INPUTCONSTANTS_H

#include <cstdint>

namespace tomato {
    /**
     * @brief Bitmask for logical player intents for a single simulation tick.
     */
    enum class InputIntent : uint32_t
    {
        None    = 0,

        // 1P
        Up      = 1 << 0,
        Down    = 1 << 1,
        Left    = 1 << 2,
        Right   = 1 << 3,

        Jump    = 1 << 4,

        TurnLeft    = 1 << 5,
        TurnRight   = 1 << 6,
        TurnUp      = 1 << 7,
        TurnDown    = 1 << 8,

        // 2P
        Up2         = 1 << 9,
        Down2       = 1 << 10,
        Left2       = 1 << 11,
        Right2      = 1 << 12,

        Jump2       = 1 << 13,

        // Debug
        Test_1  = 1U << 28,
        Test_2  = 1U << 29,
        Test_3  = 1U << 30,
        Test_4  = 1U << 31,

        COUNT
    };
}

#endif //MANGO_INPUTCONSTANTS_H