#ifndef MANGO_SYSTEMCONSTANTS_H
#define MANGO_SYSTEMCONSTANTS_H

#include <cstdint>

namespace tomato {
    enum class SystemPhase : uint8_t {
        Input = 0,
        Physics,
        Collision,
        Rendering,
        Camera,
        COUNT
    };
}

#endif //MANGO_SYSTEMCONSTANTS_H