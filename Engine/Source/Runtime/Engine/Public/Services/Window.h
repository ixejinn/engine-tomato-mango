#ifndef MANGO_WINDOW_H
#define MANGO_WINDOW_H

#include <memory>
#include "ServiceFwd.h"
#include "Tick/TickFwd.h"

struct GLFWwindow;

namespace tomato
{
    struct WindowData
    {
        Input* input;
        TickClock* tickClock;

        WindowData(Input& i, TickClock& tc)
        : input(&i), tickClock(&tc) {}
    };

    /**
     * @brief Responsible for window creation and OS-level event handling.
     */
    class Window
    {
    public:
        static int GetWidth() { return width_; }
        static int GetHeight() { return height_; }

        Window(int width, int height, const char* title);
        ~Window();

        /**
         * @brief Polls window and input events from the OS.
         */
        static void PollEvents();

        GLFWwindow* GetHandle() { return handle_; }

        [[nodiscard]] bool ShouldClose() const;
        void RequestClose() const;

        void SetWindowUserPointer(Input& i, TickClock& tc);

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

        static int width_;
        static int height_;

        GLFWwindow* handle_{nullptr};

        std::unique_ptr<WindowData> data_{nullptr};
    };
}

#endif //MANGO_WINDOW_H