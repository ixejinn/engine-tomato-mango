#ifndef MANGO_INTENTSTATE_H
#define MANGO_INTENTSTATE_H

#include <cstdint>
#include "Input/InputIntent.h"

namespace tomato {
    /**
     * @brief Snapshot of InputIntent bit states (held/down-edge) for a single simulation tick.
     *
     * 매 틱 모든 키를 스캔하지 않고, press/hold/release를 구분하기 위해
     * 현재 틱과 이전 틱의 IntentState 두 개(held/down-edge)를 저장한다.
     */
    struct IntentState
    {
        /// Simulation tick this snapshot belongs to.
        int64_t tick{0};

        /// Bits currently held down.
        /// 현재 눌려 있는 상태의 비트. (언제부터 눌려있었는지는 무관)
        InputIntent held{InputIntent::None};

        /// Bits that transitioned from up to down exactly this tick.
        /// 이번 틱에서 down으로 전환된 비트.
        InputIntent down{InputIntent::None};
    };
}

#endif //MANGO_INTENTSTATE_H