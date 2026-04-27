#ifndef MANGO_WINDOW_H
#define MANGO_WINDOW_H

#include <memory>
#include "ServiceFwd.h"
#include "Tick/TickFwd.h"

struct GLFWwindow;

namespace tomato {
    struct WindowData {
        Window* window;
        Input* input;
        TickClock* tickClock;

        WindowData(Window* w, Input* i, TickClock* tc)
        : window(w), input(i), tickClock(tc) {}
    };

    /**
     * @brief Service responsible for window creation and OS-level event handling.
     *
     * OS 및 플랫폼(GLFW)과 직접 맞닿는 계층.
     */
    class Window {
    public:
        Window(int width, int height, const char* title);
        ~Window();

        /**
         * @brief Polls window and input events from the OS.
         *
         * 키보드/마우스 콜백 등의 이벤트가 처리된다.
         */
        static void PollEvents();

        GLFWwindow* GetHandle() { return handle_; }

        int GetWidth() const { return width_; }
        int GetHeight() const { return height_; }

        [[nodiscard]] bool ShouldClose() const;
        void RequestClose() const;

        void SetWindowUserPointer(Window* w, Input* i, TickClock* tc);

        /**
         * @brief Swaps the front and back buffers.
         *
         * 렌더링이 끝난 후 프레임 단위로 호출해야 한다.
         * VSync 설정 여부에 따라 블로킹이 발생할 수 있다.
         */
        void SwapBuffers() const;

        /**
         * @brief Temporary escape-key handling helper.
         * @warning This function is temporary.
         */
        // TODO: UI System 만들면 삭제
        void TMP_CheckEscapeKey();

    private:
        static void OnFramebufferSizeChanged(GLFWwindow* window, int width, int height);

        GLFWwindow* handle_{nullptr};

        std::unique_ptr<WindowData> data_{nullptr};

        int width_;
        int height_;
    };
}

#endif //MANGO_WINDOW_H