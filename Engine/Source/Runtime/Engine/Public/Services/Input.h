#ifndef MANGO_INPUT_H
#define MANGO_INPUT_H

#include "Input/InputEvents.h"
#include "Event/EventSignal.h"
#include "ServiceFwd.h"

namespace tomato
{
    class Input
    {
    public:
        explicit Input(Window& window);

    private:
        static Key ToEngineKey(int glfwKey);
        static KeyEventType ToEngineKeyAction(int glfwAction);

        static void OnKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void OnMouseButtonEvent(GLFWwindow* window, int button, int action, int mods);
        static void OnCursorPosEvent(GLFWwindow* window, double xpos, double ypos);

        EventSignal<KeyEvent> keySignal_;
        EventSignal<MouseButtonEvent> mouseSignal_;
        EventSignal<CursorPosEvent> cursorSignal_;

        /// EventSignal::Collect에 필요한 collector
        struct InputEventCollector
        {
            bool operator()(int ret) { return !ret; }   // 리스너 함수에서 true가 반환되면 계속
        } collector_;

        friend class Engine;
    };
}

#endif //MANGO_INPUT_H