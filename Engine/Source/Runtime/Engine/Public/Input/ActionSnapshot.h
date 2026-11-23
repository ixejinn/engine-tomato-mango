#ifndef MANGO_ACTIONSNAPSHOT_H
#define MANGO_ACTIONSNAPSHOT_H

#include <cstdint>
#include "Input/InputAction.h"

namespace tomato {
    /**
     * @brief Snapshot of InputAction bit states (held/down-edge) for a single simulation tick.
     *
     * 매 틱 모든 키를 스캔하지 않고, press/hold/release를 구분하기 위해
     * 현재 틱과 이전 틱의 InputAction 두 개(held/down-edge)를 저장한다.
     */
    struct ActionSnapshot
    {
        /// Simulation tick this snapshot belongs to.
        int64_t tick{0};

        /// Bits currently held down.
        /// 현재 누르고 있는 액션. (언제부터 눌려있었는지는 무관)
        InputAction held{InputAction::None};

        /// Bits that transitioned from up to down exactly this tick.
        /// 이번 틱에서 새로 눌린 액션.
        InputAction pressed{InputAction::None};
    };
}

#endif //MANGO_ACTIONSNAPSHOT_H