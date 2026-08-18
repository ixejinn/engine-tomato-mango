#ifndef MANGO_INPUT_H
#define MANGO_INPUT_H

#include <entt/entt.hpp>
#include "Input/InputEvent.h"
#include "Event/EventSignal.h"
#include "ServiceFwd.h"
#include "TomatoFwd.h"

namespace tomato
{
    class Input
    {
    public:
        explicit Input(Window& window);

        auto GetMouseButtonSink() { return entt::sink{mouseButtonSigh_}; }
        auto GetCursorPosSink() { return entt::sink{cursorPosSigh_}; }
        auto GetScrollSink() { return entt::sink{scrollSigh_}; }
        auto GetKeySink() { return entt::sink{keySigh_}; }
        auto GetCharSink() { return entt::sink{charSigh_}; }

    private:
        static Key ConvertKeyGLFW(int glfwKey);
        static KeyAction ConvertActionGLFW(int glfwAction);

        static void OnKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void OnMouseButtonEvent(GLFWwindow* window, int button, int action, int mods);
        static void OnCursorPosEvent(GLFWwindow* window, double xpos, double ypos);
        static void OnScrollEvent(GLFWwindow* window, double xoffset, double yoffset);
        static void OnCharEvent(GLFWwindow* window, unsigned int codepoint);

        EventSignal<KeyEvent> keySignal_;
        EventSignal<MouseButtonEvent> mouseSignal_;
        EventSignal<CursorPosEvent> cursorSignal_;

        /// EventSignal::Collect에 필요한 collector
        struct InputEventCollector
        {
            bool operator()(int ret) { return !ret; }   // 리스너 함수에서 true가 반환되면 계속
        } collector_;

        // for editor
        entt::sigh<void(GLFWwindow*, int, int, int)> mouseButtonSigh_;
        entt::sigh<void(GLFWwindow*, double, double)> cursorPosSigh_;
        entt::sigh<void(GLFWwindow*, double, double)> scrollSigh_;
        entt::sigh<void(GLFWwindow*, int, int, int, int)>keySigh_;
        entt::sigh<void(GLFWwindow*, unsigned int)> charSigh_;

        friend class Engine;
    };
}

#endif //MANGO_INPUT_H