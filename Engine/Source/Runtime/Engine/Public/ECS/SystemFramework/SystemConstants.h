#ifndef MANGO_SYSTEMCONSTANTS_H
#define MANGO_SYSTEMCONSTANTS_H

#include <cstdint>

namespace tomato
{
    /// Bitmask that defines which context(s) a system is allowed to run in.
    enum class RunMode : uint8_t
    {
        None        = 0,
        Game        = 1 << 0,
        Editor      = 1 << 1,
        Rollback    = 1 << 2
    };

    /// Fixed-timestep phases executed once per simulation tick.
    enum class TickPhase : uint8_t
    {
        PreUpdate,
        Update,
        PostUpdate,
        COUNT
    };

    /// Per-frame phases executed once per rendered frame, independent of tick rate.
    enum class FramePhase : uint8_t
    {
        PreRender,
        Render,
        UI,
        PostRender,
        COUNT
    };
}

#endif //MANGO_SYSTEMCONSTANTS_H