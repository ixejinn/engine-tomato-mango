#ifndef MANGO_INPUT_H
#define MANGO_INPUT_H

#include "Input/InputEvent.h"
#include "Event/EventSignal.h"
#include "ServiceFwd.h"
#include "Containers/EnumArray.h"

struct GLFWwindow;

namespace tomato {
    class Input {
    public:
        static Key ConvertKeyGLFW(int glfwKey);
        static KeyAction ConvertActionGLFW(int glfwAction);

        static bool IsKeyPressed(Key key);
        static bool IsKeyReleased(Key key);

        explicit Input(Window& window, InputRecorder& recorder, InputUI& inputUI);

    private:
        static void OnKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void OnMouseButtonEvent(GLFWwindow* window, int button, int action, int mods);
        static void OnMouseMoveEvent(GLFWwindow* window, double xpos, double ypos);

    private:
        static inline EnumArray<Key, KeyAction> latestKeyAction_;

        EventSignal<KeyEvent> keySignal_;
        EventSignal<MouseEvent> mouseSignal_;
        EventSignal<MouseMoveEvent> moveSignal_;

        struct InputEventCollector {
            bool operator()(int ret) { return !ret; }   // 리스너 함수에서 true가 반환되면 계속
        } collector_;
    };
}

#endif //MANGO_INPUT_H