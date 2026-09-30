#ifndef MANGO_COLLISIONCONFIG_H
#define MANGO_COLLISIONCONFIG_H

namespace tomato
{
    static constexpr float RELATIVE_TOLERANCE    = 1e-3f;
    static constexpr float RELATIVE_TOLERANCE_SQ = RELATIVE_TOLERANCE * RELATIVE_TOLERANCE;
    static constexpr float NORMAL_SNAP_THRESHOLD = 1e-2f;

    static constexpr float EPSILON    = 1e-4f;
    static constexpr float EPSILON_SQ = EPSILON * EPSILON;
}

#endif //MANGO_COLLISIONCONFIG_H