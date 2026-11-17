#ifndef MANGO_INPUTEVENTS_H
#define MANGO_INPUTEVENTS_H

#include <cstdint>
#include "Input/KeyConstants.h"

namespace tomato
{
    enum class InputEvent
    {
        Released,
        Pressed,
        Repeat
    };

    /**
     * @brief Input event for a single key.
     */
    struct KeyEvent
    {
        KeyEvent(Key k, InputEvent a, float v, int64_t t)
        : key(k), event(a), value(v), tick(t) {}

        Key key;
        InputEvent event;

        float value;

        int64_t tick;
    };

    /**
     * @brief Input event for a mouse button.
     */
    struct MouseButtonEvent : public KeyEvent
    {
        MouseButtonEvent(Key k, InputEvent e, float v, int64_t t, float x, float y)
        : KeyEvent(k, e, v, t), xPos(x), yPos(y) {}

        float xPos, yPos;
    };

    /**
     * @brief A move event for a mouse.
     */
    struct CursorPosEvent
    {
        CursorPosEvent(int64_t t, float x, float y) : xPos(x), yPos(y) {}
        float xPos, yPos;
    };
}

#endif //MANGO_INPUTEVENTS_H