#ifndef MANGO_MOVEMENT_H
#define MANGO_MOVEMENT_H

#include <cstdint>

namespace tomato {
    /// Identifies which player's input timeline this entity uses.
    struct InputChannelComponent { uint8_t channel; };

    struct JumpComponent
    {
        uint8_t cnt{0};
        float vy{0.f};
    };
}

#endif //MANGO_MOVEMENT_H