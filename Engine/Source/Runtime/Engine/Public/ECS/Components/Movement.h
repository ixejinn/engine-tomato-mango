#ifndef MANGO_MOVEMENT_H
#define MANGO_MOVEMENT_H

#include <cstdint>
#include "GameObject/Character/MovementMode.h"

namespace tomato
{
    /// Identifies which player's input timeline this entity uses.
    struct InputChannelComponent
    {
        uint8_t channel{0};
        bool is1P{true};
    };

    struct MovementComponent
    {
        MovementMode mode{Falling};
        int gndStayCnt{0};
        unsigned int jumpCnt{0};
    };
}

#endif //MANGO_MOVEMENT_H