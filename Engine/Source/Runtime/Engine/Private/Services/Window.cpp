#include <stdexcept>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Services/Window.h"
#include "Utils/Logger.h"
#include "Tick/TickClock.h"

namespace tomato {
    Window::Window(const int width, const int height, const char* title)
        : width_(width), height_(height) {
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

        // VSync ON
        //glfwSwapInterval(1);

        glfwSetFramebufferSizeCallback(handle_, OnFramebufferSizeChanged);

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

    Window::~Window() {
        glfwDestroyWindow(handle_);
        glfwTerminate();
    }

    void Window::PollEvents() {
        glfwPollEvents();
    }

    bool Window::ShouldClose() const {
        return glfwWindowShouldClose(handle_);
    }

    void Window::RequestClose() const {
        glfwSetWindowShouldClose(handle_, GLFW_TRUE);
    }

    void Window::SetWindowUserPointer(Window* w, Input* i, TickClock* tc) {
        data_ = std::make_unique<WindowData>(w, i, tc);
        glfwSetWindowUserPointer(handle_, data_.get());
    }

    void Window::SwapBuffers() const {
        glfwSwapBuffers(handle_);
    }

    // !!! TEMPORAL FUNCTION !!!
    void Window::TMP_CheckEscapeKey() {
        if (glfwGetKey(handle_, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(handle_, true);
    }

    void Window::OnFramebufferSizeChanged(GLFWwindow* window, int width, int height) {
        const auto self = static_cast<WindowData*>(glfwGetWindowUserPointer(window))->window;
        self->width_ = width;
        self->height_ = height;

        glViewport(0, 0, width, height);
    }
}
