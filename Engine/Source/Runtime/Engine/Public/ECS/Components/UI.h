#ifndef MANGO_UI_H
#define MANGO_UI_H

#include <glm/vec2.hpp>

namespace tomato {
    enum class UISpace {
        Screen,
        World
    };

    // Root
    struct CanvasComponent {
        UISpace space;
    };

    // Tag
    struct UIComponent {};

    struct RectComponent {
        float width;
        float height;

        glm::vec2 pivot;
        glm::vec2 anchorMin;
        glm::vec2 anchorMax;
    };
}

#endif //MANGO_UI_H