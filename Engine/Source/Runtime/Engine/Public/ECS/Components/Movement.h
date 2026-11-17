#ifndef MANGO_MOVEMENT_H
#define MANGO_MOVEMENT_H

#include <cstdint>
#include "GameObject/Character/MovementMode.h"

namespace tomato
{
    /**
     * @brief Marks an entity as controlled by a player.
     */
    struct PlayerControlledComponent
    {
        uint8_t playerIdx{0};
        bool useWASD{true};
    };

    struct MovementComponent
    {
        int gndStayCnt{0};
        unsigned int jumpCnt{0};

        MovementMode mode{Falling};
        InputControl control{Direct};
    };
}

#endif //MANGO_MOVEMENT_H