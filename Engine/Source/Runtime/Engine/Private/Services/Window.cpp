#include <stdexcept>
#include <chrono>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include "Services/Window.h"
#include "Services/ChangeFramebufferSizeEvent.h"
#include "Utils/Logger.h"
#include "Simulation/Tick/TickClock.h"
#include "Event/EventDispatcher.h"

namespace tomato
{
    int Window::width_ = 0;
    int Window::height_ = 0;

    namespace
    {
        /**
         * @param scancode 키보드 하드웨어의 키 번호(GLFW로부터 얻을 수 있음)
         * @return Windows에서 사용하는 가상키
         * @note NumLock 상태는 반영되지 않음 (꺼진 상태로 반환됨)
         */
        UINT ToVirtualKey(const int scancode)
        {
            const UINT extendedPrefix = (scancode & KF_EXTENDED) ? 0xE000 : 0;  // 확장키(방향키 등)인 경우
            return MapVirtualKeyW((scancode & 0xFF) | extendedPrefix, MAPVK_VSC_TO_VK_EX);
        }

        bool IsVirtualKeyDown(const int virtualKey)
        {
            return (GetAsyncKeyState(virtualKey) & 0x8000) != 0;    // 최상위 비트가 0이 아니면 눌려있음
        }

        bool IsPhysicalKeyDown(const int glfwKey, const UINT virtualKey)
        {
            if (glfwKey >= GLFW_KEY_KP_0 && glfwKey <= GLFW_KEY_KP_9)   // 넘패드 숫자
                return IsVirtualKeyDown(VK_NUMPAD0 + (glfwKey - GLFW_KEY_KP_0));

            if (glfwKey == GLFW_KEY_KP_DECIMAL)                         // 넘패드 .
                return IsVirtualKeyDown(VK_DECIMAL);

            return virtualKey == 0 || IsVirtualKeyDown(static_cast<int>(virtualKey));
        }
    }

    Window::Window(const int width, const int height, const char* title)
    {
        width_ = width;
        height_ = height;

        // [GLFW] Initialize and configure
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        handle_ = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (handle_ == nullptr)
        {
            glfwDestroyWindow(handle_);
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }
        glfwMakeContextCurrent(handle_);

        glfwSetFramebufferSizeCallback(handle_, OnFramebufferSizeChanged);
        EventDispatcher::GetInstance().Connect<ChangeFramebufferSizeEvent>();

        // [GLAD] load all OpenGL function pointers
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            glfwDestroyWindow(handle_);
            glfwTerminate();
            handle_ = nullptr;
            throw std::runtime_error("Failed to initialize GLAD");
        }

        const GLubyte* version = glGetString(GL_VERSION);
        if (version)
            TMT_INFO << "OpenGL version: " << (const char*)version;
        else
            TMT_INFO << "Failed to get OpenGL version information";
    }

    Window::~Window()
    {
        glfwDestroyWindow(handle_);
        glfwTerminate();
    }

    void Window::SetVSync(bool set)
    {
        if (set)
            glfwSwapInterval(1);
        else
            glfwSwapInterval(0);
    }

    void Window::PollEvents()
    {
        constexpr std::chrono::milliseconds LOST_RELEASE_CHECK_THRESHOLD{50};

        const auto begin = std::chrono::steady_clock::now();
        glfwPollEvents();
        if (std::chrono::steady_clock::now() - begin >= LOST_RELEASE_CHECK_THRESHOLD)
            RecoverLostKeyRelease();
    }

    bool Window::ShouldClose() const
    {
        return glfwWindowShouldClose(handle_);
    }

    void Window::RequestClose() const
    {
        glfwSetWindowShouldClose(handle_, GLFW_TRUE);
    }

    void Window::SetWindowUserPointer(Input& i, TickClock& tc)
    {
        data_ = std::make_unique<WindowData>(i, tc);
        glfwSetWindowUserPointer(handle_, data_.get());
    }

    void Window::SwapBuffers() const
    {
        glfwSwapBuffers(handle_);
    }

    // !!! TEMPORAL FUNCTION !!!
    void Window::TMP_CheckEscapeKey()
    {
        if (glfwGetKey(handle_, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(handle_, true);
    }

    void Window::SetCursorDisable(bool disable)
    {
        if (disable)
            glfwSetInputMode(handle_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else
            glfwSetInputMode(handle_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    void Window::OnFramebufferSizeChanged(GLFWwindow* window, int width, int height)
    {
        width_ = width;
        height_ = height;

        glViewport(0, 0, width, height);

        EventDispatcher::GetInstance().Enqueue(ChangeFramebufferSizeEvent{width, height});
    }

    void Window::RecoverLostKeyRelease() const
    {
        const HWND hwnd = glfwGetWin32Window(handle_);

        for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key)
        {
            if (glfwGetKey(handle_, key) != GLFW_PRESS)
                continue;
            // GLFW에서 GLFW_PRESS인 키들만 복구 후보

            const int scancode = glfwGetKeyScancode(key);
            if (scancode < 0)
                continue;

            const UINT virtualKey = ToVirtualKey(scancode);
            if (IsPhysicalKeyDown(key, virtualKey))
                continue;

            // 놓친 release 메시지 재생성
            const WORD keyFlags = static_cast<WORD>(scancode | KF_REPEAT | KF_UP);
            SendMessageW(hwnd, WM_KEYUP, virtualKey, MAKELPARAM(1, keyFlags));
        }
    }
}
