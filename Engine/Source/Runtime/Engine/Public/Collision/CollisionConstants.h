#ifndef MANGO_COLLISIONCONSTANTS_H
#define MANGO_COLLISIONCONSTANTS_H

namespace tomato
{
    static constexpr float COLLISION_SKIN = 1e-2f;
    static constexpr float HALF_COLLISION_SKIN = 0.5e-2f;

    constexpr float SAFETY_COEF = 1.28f;
    constexpr float SAFETY_COEF_SQ = SAFETY_COEF * SAFETY_COEF;
}

#endif //MANGO_COLLISIONCONSTANTS_H