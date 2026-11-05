#include <GLFW/glfw3.h>
#include "Services/Input.h"
#include "Services/Window.h"
#include "Input/KeyDeviceState.h"
#include "Simulation/Tick/TickClock.h"
#include "Utils/Logger.h"

namespace tomato
{
    Input::Input(Window& window)
    {
        auto* windowHandle = window.GetHandle();
        glfwSetKeyCallback(windowHandle, OnKeyEvent);
        glfwSetMouseButtonCallback(windowHandle, OnMouseButtonEvent);
        glfwSetCursorPosCallback(windowHandle, OnCursorPosEvent);
        glfwSetScrollCallback(windowHandle, OnScrollEvent);
        glfwSetCharCallback(windowHandle, OnCharEvent);
    }

    void Input::OnKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        Key k = ConvertKeyGLFW(key);
        KeyAction a = ConvertActionGLFW(action);
        if (a == KeyAction::Error)
        {
            TMT_WARN << "KeyAction of " << static_cast<int>(k) << " is invalid";
            return;
        }

        auto* winData = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
        auto* input = winData->input;
        auto* tickClock = winData->tickClock;

        const float v = (a == KeyAction::Release) ? 0.0f : 1.0f;
        KeyDeviceState::GetInstance().UpdateInputAxis(k, v);

        input->keySigh_.publish(window, key, scancode, action, mods);   // ImGui에서 입력 소비 여부 결정

        const KeyEvent event{k, a, v, tickClock->GetTick()};
        if (a == KeyAction::Release)
            input->keySignal_.Publish(event);                           // Release 이벤트는 모두에게 보냄
        else
            input->keySignal_.Collect(input->collector_, event);    // 에디터-UI-게임 순으로 입력 우선 소비
    }

    void Input::OnMouseButtonEvent(GLFWwindow* window, int button, int action, int mods)
    {
        Key k = ConvertKeyGLFW(button);
        KeyAction a = ConvertActionGLFW(action);

        if (a == KeyAction::Error)
        {
            TMT_WARN << "KeyAction of " << static_cast<int>(k) << " is invalid";
            return;
        }

        auto* winData = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
        auto* input = winData->input;
        auto* tickClock = winData->tickClock;

        double xPos(0), yPos(0);
        glfwGetCursorPos(window, &xPos, &yPos);

        const float v = (a == KeyAction::Release) ? 0.0f : 1.0f;
        KeyDeviceState::GetInstance().UpdateInputAxis(k, v);

        input->mouseButtonSigh_.publish(window, button, action, mods);

        const MouseButtonEvent event{k, a, v, tickClock->GetTick(), static_cast<float>(xPos), static_cast<float>(yPos)};
        if (a == KeyAction::Release)
            input->mouseSignal_.Publish(event);
        else
            input->mouseSignal_.Collect(input->collector_, event);

    }

    void Input::OnCursorPosEvent(GLFWwindow* window, double xpos, double ypos)
    {
        auto* winData = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
        auto* input = winData->input;
        auto* tickClock = winData->tickClock;

        input->cursorPosSigh_.publish(window, xpos, ypos);
        input->cursorSignal_.Publish(
            CursorPosEvent{tickClock->GetTick(), static_cast<float>(xpos), static_cast<float>(ypos)});

        // if (externalCallbacks_.mouseMove)
        //     externalCallbacks_.mouseMove(window, xpos, ypos);

        KeyDeviceState::GetInstance().UpdateInputAxis(Key::MouseX, xpos);
        KeyDeviceState::GetInstance().UpdateInputAxis(Key::MouseY, ypos);
    }
    
    void Input::OnScrollEvent(GLFWwindow* window, double xoffset, double yoffset)
    {
        static_cast<WindowData*>(glfwGetWindowUserPointer(window))->input->scrollSigh_.publish(window, xoffset, yoffset);
    }

    void Input::OnCharEvent(GLFWwindow* window, unsigned int codepoint)
    {
        static_cast<WindowData*>(glfwGetWindowUserPointer(window))->input->charSigh_.publish(window, codepoint);
    }

    Key Input::ConvertKeyGLFW(int glfwKey)
    {
        switch (glfwKey)
        {
        case GLFW_MOUSE_BUTTON_LEFT:
            return Key::LeftMouseButton;
        case GLFW_MOUSE_BUTTON_RIGHT:
            return Key::RightMouseButton;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            return Key::MiddleMouseButton;

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
            return KeyAction::Press;
        case GLFW_REPEAT:
            return KeyAction::Repeat;
        default:
            return KeyAction::Error;
        }
    }
}