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
        KeyEvent(Key k, KeyAction a, float v, uint32_t t)
        : key(k), action(a), value(v), tick(t) {}

        Key key;
        KeyAction action;

        float value;

        uint32_t tick;
    };

    /**
     * @brief Input event for a mouse.
     */
    struct MouseEvent : public KeyEvent
    {
        MouseEvent(Key k, KeyAction a, float v, uint32_t t, float x, float y)
        : KeyEvent(k, a, v, t), xPos(x), yPos(y) {}

        float xPos, yPos;
    };
}

#endif //MANGO_INPUTEVENT_H