#ifndef MANGO_INPUTRECORD_H
#define MANGO_INPUTRECORD_H

#include <cstdint>
#include "Input/InputConstants.h"

namespace tomato {
    /**
     * @brief Per-tick input snapshot used by the simulation (and rollback).
     *
     * 특정 틱에서 한 플레이어의 입력 스냅샷.
     */
    struct InputRecord
    {
        int64_t tick{0};

        /// Key state (held)
        InputIntent held{InputIntent::None};
        /// Key down edge (up → down)
        InputIntent down{InputIntent::None};
    };
}

#endif //MANGO_INPUTRECORD_H