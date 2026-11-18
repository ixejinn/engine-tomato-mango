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
        /// State의 PlayerInputTimelines에 접근할 때 사용하는 인덱스 (NM_Client일 때는 무시됨)
        uint8_t playerIdx{0};
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