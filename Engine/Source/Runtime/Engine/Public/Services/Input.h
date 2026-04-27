#ifndef MANGO_INPUT_H
#define MANGO_INPUT_H

#include "Input/InputEvent.h"
#include "Event/EventSignal.h"
#include "ServiceFwd.h"

struct GLFWwindow;

namespace tomato {
    class Input {
    public:
        static Key ConvertKeyGLFW(int glfwKey);
        static KeyAction ConvertActionGLFW(int glfwAction);

        explicit Input(Window& window, InputRecorder& recorder);

    private:
        static void OnKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void OnMouseButtonEvent(GLFWwindow* window, int button, int action, int mods);

        EventSignal<KeyEvent> keySignal_;
        EventSignal<MouseEvent> mouseSignal_;
        struct InputEventCollector {
            bool operator()(int ret) { return !ret; }   // 리스너 함수에서 true가 반환되면 계속
        } collector_;
    };
}

#endif //MANGO_INPUT_H