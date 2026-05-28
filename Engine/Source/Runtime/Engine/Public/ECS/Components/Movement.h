#ifndef MANGO_MOVEMENT_H
#define MANGO_MOVEMENT_H

#include <cstdint>

namespace tomato {
    /// Identifies which player's input timeline this entity uses.
    struct InputChannelComponent {
        uint8_t channel;
        bool is1P = true;
    };

    struct MovementComponent
    {
        float horizontalSpeed{2.f};
        uint8_t jumpCnt{0};
    };
}

#endif //MANGO_MOVEMENT_H