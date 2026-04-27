#ifndef MANGO_ENGINECONFIG_H
#define MANGO_ENGINECONFIG_H

namespace tomato {
    constexpr int FRAME_PER_SECOND{60};
    constexpr float FIXED_DELTA_TIME{1.f / FRAME_PER_SECOND};

    constexpr int MAX_SIMULATION_NUM_PER_FRAME{3};
}

#endif //MANGO_ENGINECONFIG_H