#include "Services/Input.h"
#include "Services/Window.h"
#include "Input/InputRecorder.h"
#include "Input/InputUI.h"
#include "Tick/TickClock.h"
#include "Utils/Logger.h"
#include <GLFW/glfw3.h>

namespace tomato
{
    InputCallbacks Input::externalCallbacks_;

    Key Input::ConvertKeyGLFW(int glfwKey)
    {
        switch (glfwKey)
        {
        case GLFW_MOUSE_BUTTON_LEFT:
            return Key::LeftMouseButton;
        case GLFW_MOUSE_BUTTON_RIGHT:
            return Key::RightMouseButton;

        case GLFW_KEY_ESCAPE:
            return Key::Escape;
        case GLFW_KEY_SPACE:
            return Key::SpaceBar;

        case GLFW_KEY_LEFT:
            return Key::Left;
        case GLFW_KEY_UP:
            return Key::Up;
        case GLFW_KEY_RIGHT:
            return Key::Right;
        case GLFW_KEY_DOWN:
            return Key::Down;

        case GLFW_KEY_0:
            return Key::K_0;
        case GLFW_KEY_1:
            return Key::K_1;
        case GLFW_KEY_2:
            return Key::K_2;
        case GLFW_KEY_3:
            return Key::K_3;
        case GLFW_KEY_4:
            return Key::K_4;
        case GLFW_KEY_5:
            return Key::K_5;
        case GLFW_KEY_6:
            return Key::K_6;
        case GLFW_KEY_7:
            return Key::K_7;
        case GLFW_KEY_8:
            return Key::K_8;
        case GLFW_KEY_9:
            return Key::K_9;

        case GLFW_KEY_A:
            return Key::A;
        case GLFW_KEY_B:
            return Key::B;
        case GLFW_KEY_C:
            return Key::C;
        case GLFW_KEY_D:
            return Key::D;
        case GLFW_KEY_E:
            return Key::E;
        case GLFW_KEY_F:
            return Key::F;
        case GLFW_KEY_G:
            return Key::G;
        case GLFW_KEY_H:
            return Key::H;
        case GLFW_KEY_I:
            return Key::I;
        case GLFW_KEY_J:
            return Key::J;
        case GLFW_KEY_K:
            return Key::K;
        case GLFW_KEY_L:
            return Key::L;
        case GLFW_KEY_M:
            return Key::M;
        case GLFW_KEY_N:
            return Key::N;
        case GLFW_KEY_O:
            return Key::O;
        case GLFW_KEY_P:
            return Key::P;
        case GLFW_KEY_Q:
            return Key::Q;
        case GLFW_KEY_R:
            return Key::R;
        case GLFW_KEY_S:
            return Key::S;
        case GLFW_KEY_T:
            return Key::T;
        case GLFW_KEY_U:
            return Key::U;
        case GLFW_KEY_V:
            return Key::V;
        case GLFW_KEY_W:
            return Key::W;
        case GLFW_KEY_X:
            return Key::X;
        case GLFW_KEY_Y:
            return Key::Y;
        case GLFW_KEY_Z:
            return Key::Z;

        case GLFW_KEY_KP_0:
            return Key::KP_0;
        case GLFW_KEY_KP_1:
            return Key::KP_1;
        case GLFW_KEY_KP_2:
            return Key::KP_2;
        case GLFW_KEY_KP_3:
            return Key::KP_3;
        case GLFW_KEY_KP_4:
            return Key::KP_4;
        case GLFW_KEY_KP_5:
            return Key::KP_5;
        case GLFW_KEY_KP_6:
            return Key::KP_6;
        case GLFW_KEY_KP_7:
            return Key::KP_7;
        case GLFW_KEY_KP_8:
            return Key::KP_8;
        case GLFW_KEY_KP_9:
            return Key::KP_9;

        default:
            return Key::UNKNOWN;
        }
    }

    KeyAction Input::ConvertActionGLFW(int glfwAction)
    {
        switch (glfwAction)
        {
        case GLFW_RELEASE:
            return KeyAction::Release;
        case GLFW_PRESS:
        case GLFW_REPEAT:
            return KeyAction::Press;
        default:
            return KeyAction::COUNT;
        }
    }

    bool Input::IsKeyPressed(Key key)
    {
        return latestKeyAction_[key] == KeyAction::Press;
    }

    bool Input::IsKeyReleased(Key key)
    {
        return latestKeyAction_[key] == KeyAction::Release;
    }

    Input::Input(Window& window, InputRecorder& recorder, InputUI& inputUI) {
        glfwSetKeyCallback(window.GetHandle(), OnKeyEvent);
        glfwSetMouseButtonCallback(window.GetHandle(), OnMouseButtonEvent);
        glfwSetCursorPosCallback(window.GetHandle(), OnMouseMoveEvent);
        glfwSetScrollCallback(window.GetHandle(), OnScrollEvent);
        glfwSetCharCallback(window.GetHandle(), OnCharacterEvent);

        keySignal_.Connect<&InputRecorder::UpdateInputKey>(recorder);
        moveSignal_.Connect<&InputUI::OnHover>(inputUI);
        mouseSignal_.Connect<&InputRecorder::UpdateInputMouse>(recorder);
        mouseSignal_.Connect<&InputUI::OnClick>(inputUI);
    }

    void Input::SetExternalInputCallbacks(InputCallbacks cb)
    {
        externalCallbacks_ = cb;
    }

    void Input::OnKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (externalCallbacks_.key)
        {
            externalCallbacks_.key(window, key, scancode, action, mods);
            //return; @TODO : if editor mode
        }
        Key k = ConvertKeyGLFW(key);
        KeyAction a = ConvertActionGLFW(action);

        if (a >= KeyAction::COUNT)
        {
            TMT_WARN << "KeyAction of " << static_cast<int>(k) << " is invalid";
            return;
        }
        latestKeyAction_[k] = a;

        auto* winData = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
        auto* input = winData->input;
        auto* tickClock = winData->tickClock;
        input->keySignal_.Collect(input->collector_,
                               KeyEvent{k, a, a == KeyAction::Release ? 0.f : 1.f, tickClock->GetTick()});

    }

    void Input::OnMouseButtonEvent(GLFWwindow* window, int button, int action, int mods)
    {
        Key k = ConvertKeyGLFW(button);
        KeyAction a = ConvertActionGLFW(action);

        if (a >= KeyAction::COUNT)
        {
            TMT_WARN << "Invalid KeyAction " << action;
            return;
        }

        latestKeyAction_[k] = a;

        auto* winData = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
        auto* input = winData->input;
        auto* tickClock = winData->tickClock;

        double xPos(0), yPos(0);
        glfwGetCursorPos(window, &xPos, &yPos);

        input->mouseSignal_.Collect(input->collector_,
                                   MouseEvent{k, a, a == KeyAction::Release ? 0.f : 1.f, tickClock->GetTick(), static_cast<float>(xPos), static_cast<float>(yPos)});
    
        if (externalCallbacks_.mouseButton)
            externalCallbacks_.mouseButton(window, button, action, mods);
    }

    void Input::OnMouseMoveEvent(GLFWwindow* window, double xpos, double ypos)
    {
        auto* winData = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
        auto* input = winData->input;
        auto* tickClock = winData->tickClock;

        input->moveSignal_.Collect(input->collector_,
            MouseMoveEvent{ tickClock->GetTick(), static_cast<float>(xpos), static_cast<float>(ypos) });

        if (externalCallbacks_.mouseMove)
            externalCallbacks_.mouseMove(window, xpos, ypos);
    }
    
    void Input::OnScrollEvent(GLFWwindow* window, double xoffset, double yoffset)
    {
        if (externalCallbacks_.scroll)
            externalCallbacks_.scroll(window, xoffset, yoffset);
    }

    void Input::OnCharacterEvent(GLFWwindow* window, unsigned int codepoint)
    {
        if (externalCallbacks_.character)
            externalCallbacks_.character(window, codepoint);
    }
}