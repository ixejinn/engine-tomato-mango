#include "Input/KeyDeviceState.h"
#include "Input/InputEvents.h"

namespace tomato
{
    bool KeyDeviceState::OnKeyEvent(const tomato::KeyEvent &event)
    {
        if (event.action != KeyAction::Repeat)
            UpdateInputAxis(event.key, event.value);
        return true;
    }

    bool KeyDeviceState::OnMouseButtonEvent(const MouseButtonEvent& event)
    {
        if (event.action != KeyAction::Repeat)
            UpdateInputAxis(event.key, event.value);
        return true;
    }

    bool KeyDeviceState::OnCursorPosEvent(const CursorPosEvent& event)
    {
        UpdateInputAxis(Key::MouseX, event.xPos);
        UpdateInputAxis(Key::MouseY, event.yPos);
        return true;
    }

    void KeyDeviceState::UpdateInputAxis(Key key, InputAxis value)
    {
        latestValues_[key] = value;
    }

    bool KeyDeviceState::IsKeyPressed(Key key) const
    {
        if (key < Key::AXIS_BEGIN && latestValues_[key] > 0.5f)
            return true;
        return false;
    }

    bool KeyDeviceState::IsKeyReleased(Key key) const
    {
        if (key < Key::AXIS_BEGIN && latestValues_[key] < 0.5f)
            return true;
        return false;
    }
}
