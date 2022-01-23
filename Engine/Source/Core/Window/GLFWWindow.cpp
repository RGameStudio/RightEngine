#include "GLFWWindow.hpp"
#include <GLFW/glfw3.h>
#include "Core.h"

namespace RightEngine
{
    GLFWWindow::GLFWWindow(std::string title, uint32_t width, uint32_t height) : Window(std::move(title), width, height)
    {
        Init(title, width, height);
    }

    void GLFWWindow::Init(std::string title, uint32_t width, uint32_t height)
    {
        R_CORE_ASSERT(glfwInit(), "GLFW init failed");

        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        glfwWindowHint(GLFW_SAMPLES, 4);

        window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        R_CORE_ASSERT(window, "GLFW window create failed");

        glfwMakeContextCurrent(window);
    }

    void GLFWWindow::OnUpdate()
    {
        glfwPollEvents();
    }

    void GLFWWindow::Swap()
    {
        glfwSwapBuffers(window);
    }

    GLFWWindow::~GLFWWindow()
    {
        glfwDestroyWindow(window);
    }
}
