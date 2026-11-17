#include "Input/KeyStateMap.h"
#include "Input/InputEvents.h"

namespace tomato
{
    bool KeyStateMap::OnKeyEvent(const KeyEvent &event)
    {
        if (event.event != InputEvent::Repeat)
            UpdateInputAxis(event.key, event.value);
        return true;
    }

    bool KeyStateMap::OnMouseButtonEvent(const MouseButtonEvent& event)
    {
        if (event.event != InputEvent::Repeat)
            UpdateInputAxis(event.key, event.value);
        return true;
    }

    bool KeyStateMap::OnCursorPosEvent(const CursorPosEvent& event)
    {
        UpdateInputAxis(Key::MouseX, event.xPos);
        UpdateInputAxis(Key::MouseY, event.yPos);
        return true;
    }

    void KeyStateMap::UpdateInputAxis(const Key key, const InputAxis value)
    {
        latestValues_[key] = value;
    }

    bool KeyStateMap::IsKeyDown(const Key key) const
    {
        return IsButton(key) && latestValues_[key] > 0.5f;
    }

    bool KeyStateMap::IsKeyUp(const Key key) const
    {
        return IsButton(key) && latestValues_[key] < 0.5f;
    }
}
