#pragma once

#include <Window.hpp>
#include <Windows.h>

class WindowsWindow: public Window
{
friend class Window;

private:
    WindowsWindow(std::string title, uint32_t width, uint32_t height);
    HWND windowHandle = nullptr;
};