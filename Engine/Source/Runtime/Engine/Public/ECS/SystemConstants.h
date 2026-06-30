#ifndef MANGO_SYSTEMCONSTANTS_H
#define MANGO_SYSTEMCONSTANTS_H

#include <cstdint>

namespace tomato
{
    enum class SystemPhase : uint8_t
    {
        Input = 0,
        Physics,
        Collision,
        OnTrigger,
        OnCollision,
        Integration,
        Transformation,
        Camera,
        UI,
        UITransformation,
        Rendering,
        ScreenUI,

        GameLogic,
        COUNT
    };

    constexpr SystemPhase SIMULATION_ORDER[]
    {
        SystemPhase::Input,
        SystemPhase::Physics,
        SystemPhase::Collision,
        SystemPhase::OnTrigger,
        SystemPhase::OnCollision,
        SystemPhase::Integration,
        SystemPhase::Transformation,
    };

    constexpr SystemPhase RENDERING_ORDER[]
    {
        SystemPhase::Camera,
        SystemPhase::UI,
        SystemPhase::UITransformation,
        SystemPhase::Rendering,
        SystemPhase::ScreenUI
    };
}

#endif //MANGO_SYSTEMCONSTANTS_H