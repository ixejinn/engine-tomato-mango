#ifndef MANGO_INPUTCONSTANTS_H
#define MANGO_INPUTCONSTANTS_H

namespace tomato {
    /**
     * @brief Bitmask for logical player intents for a single simulation tick.
     */
    enum class InputIntent
    {
        None    = 0,

        Up      = 1 << 0,
        Down    = 1 << 1,
        Left    = 1 << 2,
        Right   = 1 << 3,

        Jump    = 1 << 4,

        // Debug
        Test_1  = 1 << 5,
        Test_2  = 1 << 6,
        Test_3  = 1 << 7,
        Test_4  = 1 << 8,

        COUNT
    };
}

#endif //MANGO_INPUTCONSTANTS_H