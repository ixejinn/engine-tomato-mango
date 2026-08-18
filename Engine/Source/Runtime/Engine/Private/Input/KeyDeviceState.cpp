#include "Input/KeyDeviceState.h"
#include "Input/InputEvent.h"

namespace tomato
{
    void KeyDeviceState::UpdateInputAxis(Key key, InputAxis value)
    {
        latestValues_[key] = value;
    }

    bool KeyDeviceState::IsKeyPressed(Key key)
    {
        if (key < Key::AXIS_BEGIN && latestValues_[key] > 0.5f)
            return true;
        return false;
    }

    bool KeyDeviceState::IsKeyReleased(Key key)
    {
        if (key < Key::AXIS_BEGIN && latestValues_[key] < 0.5f)
            return true;
        return false;
    }
}
