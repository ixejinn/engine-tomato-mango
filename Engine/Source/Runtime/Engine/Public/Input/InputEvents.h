#ifndef MANGO_INPUTEVENTS_H
#define MANGO_INPUTEVENTS_H

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
        KeyEvent(Key k, InputEvent a, float v) : key(k), event(a), value(v) {}

        Key key;
        InputEvent event;
        float value;
    };

    /**
     * @brief A move event for a mouse.
     */
    struct CursorPosEvent
    {
        CursorPosEvent(float x, float y) : xPos(x), yPos(y) {}

        float xPos, yPos;
    };
}

#endif //MANGO_INPUTEVENTS_H