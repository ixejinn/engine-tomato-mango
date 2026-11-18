#ifndef MANGO_LOCALPLAYER_H
#define MANGO_LOCALPLAYER_H

#include "Input/PlayerInput.h"

namespace tomato
{
    struct LocalPlayer
    {
        LocalPlayer(int index) : playerIdx(index) {}

        /// State의 PlayerInputTimelines에 접근할 때 사용하는 인덱스 (NM_Client일 때는 무시됨)
        int playerIdx;
        PlayerInput playerInput;
    };
}

#endif //MANGO_LOCALPLAYER_H