#include "Input/InputState.h"
#include "Input/InputEvents.h"

namespace tomato
{
    bool InputState::OnKeyEvent(const tomato::KeyEvent &event)
    {
        if (event.type != KeyEventType::Repeat)
            UpdateInputAxis(event.key, event.value);
        return true;
    }

    bool InputState::OnMouseButtonEvent(const MouseButtonEvent& event)
    {
        if (event.type != KeyEventType::Repeat)
            UpdateInputAxis(event.key, event.value);
        return true;
    }

    bool InputState::OnCursorPosEvent(const CursorPosEvent& event)
    {
        UpdateInputAxis(Key::MouseX, event.xPos);
        UpdateInputAxis(Key::MouseY, event.yPos);
        return true;
    }

    void InputState::UpdateInputAxis(Key key, InputAxis value)
    {
        latestValues_[key] = value;
    }

    bool InputState::IsKeyPressed(Key key) const
    {
        if (key < Key::AXIS_BEGIN && latestValues_[key] > 0.5f)
            return true;
        return false;
    }

    bool InputState::IsKeyReleased(Key key) const
    {
        if (key < Key::AXIS_BEGIN && latestValues_[key] < 0.5f)
            return true;
        return false;
    }
}
