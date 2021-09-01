#include "Window.hpp"

#include <utility>
#include "WindowsWindow.hpp"

namespace RightEngine
{
    Window *Window::Create(std::string title, uint32_t width, uint32_t height)
    {
        return new WindowsWindow(std::move(title), width, height);
    }
}