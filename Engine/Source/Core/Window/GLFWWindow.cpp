#include "GLFWWindow.hpp"
#include "Core.h"
#include "MouseEvent.hpp"

namespace RightEngine
{
    GLFWWindow::GLFWWindow(const std::string& title, uint32_t width, uint32_t height) : Window(title, width, height)
    {
        Init(title, width, height);
    }

    void GLFWWindow::Init(const std::string& title, uint32_t width, uint32_t height)
    {
        R_CORE_ASSERT(glfwInit(), "GLFW init failed");

        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR_VERSION);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR_VERSION);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        R_CORE_ASSERT(window, "GLFW window create failed");

        glfwSetWindowCloseCallback(window, [](GLFWwindow* window)
        {
            EventDispatcher::Get()->Emit(ShutdownEvent());
        });

        glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xPos, double yPos)
        {
            EventDispatcher::Get()->Emit(MouseMovedEvent(xPos, yPos));
        });
    }

    void GLFWWindow::OnUpdate()
    {
        glfwPollEvents();
    }

    void GLFWWindow::Swap() const
    {
        glfwSwapBuffers(window);
    }

    GLFWWindow::~GLFWWindow()
    {
        glfwDestroyWindow(window);
    }

    void* GLFWWindow::GetNativeHandle() const
    {
        return window;
    }
}
