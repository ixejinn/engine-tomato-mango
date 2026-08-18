#ifndef MANGO_INPUTEVENT_H
#define MANGO_INPUTEVENT_H

#include <cstdint>
#include "Input/KeyConstants.h"

namespace tomato {

    /**
     * @brief Input event for a single key.
     */
    struct KeyEvent
    {
        KeyEvent(Key k, KeyAction a, float v, int64_t t)
        : key(k), action(a), value(v), tick(t) {}

        Key key;
        KeyAction action;

        float value;

        int64_t tick;
    };

    /**
     * @brief Input event for a mouse.
     */
    struct MouseButtonEvent : public KeyEvent
    {
        MouseButtonEvent(Key k, KeyAction a, float v, int64_t t, float x, float y)
        : KeyEvent(k, a, v, t), xPos(x), yPos(y) {}

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

#endif //MANGO_INPUTEVENT_H