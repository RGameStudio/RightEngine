#include "Window.hpp"
#include <utility>

#ifdef WIN32
#include "WindowsWindow.hpp"
#else
#include "GLFWWindow.hpp"
#endif

namespace RightEngine
{
    Window *Window::Create(std::string title, uint32_t width, uint32_t height)
    {
#ifdef WIN32
        return new WindowsWindow(std::move(title), width, height);
#else
        return new GLFWWindow(std::move(title), width, height);
#endif
    }
}