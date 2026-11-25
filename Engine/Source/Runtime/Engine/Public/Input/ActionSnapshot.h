#ifndef MANGO_ACTIONSNAPSHOT_H
#define MANGO_ACTIONSNAPSHOT_H

#include <cstdint>
#include "Input/InputAction.h"

namespace tomato {
    /**
     * @brief State of one player's InputActions for a single simulation tick.
     *
     * State의 PlayerInputTimelines에 저장되고 네트워크로 전송되는 단위.
     * 시뮬레이션 중 플레이어 캐릭터 조작에 사용된다.
     */
    struct ActionSnapshot
    {
        /// Simulation tick this snapshot belongs to.
        int64_t tick{0};

        /// Bits currently held down.
        /// 현재 누르고 있는 액션. (언제부터 눌려있었는지는 무관)
        InputAction down{InputAction::None};

        /// Bits that transitioned from up to down exactly this tick.
        /// 이번 틱에서 새로 눌린 액션.
        InputAction pressed{InputAction::None};
    };
}

#endif //MANGO_ACTIONSNAPSHOT_H