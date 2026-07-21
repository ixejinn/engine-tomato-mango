#ifndef MANGO_MOVEMENT_H
#define MANGO_MOVEMENT_H

#include <cstdint>
#include "GameObjects/Character/MovementMode.h"

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
        float horizontalSpeed{2.f};

        MovementMode mode{Falling};
        int gndStayCnt{0};
        unsigned int jumpCnt{0};
    };
}

#endif //MANGO_MOVEMENT_H