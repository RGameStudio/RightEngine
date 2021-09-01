#pragma once

#include <Window.hpp>
#include <Windows.h>

namespace RightEngine
{
    class WindowsWindow : public Window
    {
        friend class Window;
    public:

        virtual void OnUpdate() override;

    private:
        WindowsWindow(std::string title, uint32_t width, uint32_t height);

        HWND windowHandle = nullptr;
        HINSTANCE appInstance = nullptr;
    };
}