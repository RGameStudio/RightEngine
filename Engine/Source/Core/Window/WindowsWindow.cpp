#include "WindowsWindow.hpp"
#include "Core.h"

namespace RightEngine
{
    LRESULT CALLBACK WndProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

    WindowsWindow::WindowsWindow(std::string title, uint32_t width, uint32_t height) : Window(title, width, height)
    {
        const char *windowClass = "EngineWindowClass";
        appInstance = GetModuleHandle(nullptr);

        WNDCLASSEX wc;
        ZeroMemory(&wc, sizeof(WNDCLASSEX));

        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WndProc;
        wc.hInstance = appInstance;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH) COLOR_WINDOW;
        wc.lpszClassName = windowClass;

        RegisterClassEx(&wc);

        windowHandle = CreateWindowEx(
                0x0,                  // Optional window styles.
                windowClass,          // Window class
                "RightEngine2D",      // Window text
                WS_OVERLAPPEDWINDOW,  // Window style
                // Size and position
                CW_USEDEFAULT, CW_USEDEFAULT, width, height,
                nullptr,              // Parent window
                nullptr,              // Menu
                appInstance,          // Instance handle
                nullptr               // Additional application data
        );

        if (!windowHandle)
        {
            R_CORE_ASSERT(windowHandle, "Can't create engine window!");
        }

        ShowWindow(windowHandle, SW_SHOW);

        R_CORE_INFO("Created window successfully! Size: {0}x{1}", width, height);
    }

    void WindowsWindow::OnUpdate()
    {
        MSG message;
        while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
    }

    LRESULT CALLBACK WndProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
            case WM_KEYDOWN:
                if (wParam == VK_ESCAPE)
                {
                    EventDispatcher::Get()->Emit(ShutdownEvent());
                }
                return 0;
            case WM_DESTROY:
                PostQuitMessage(0);
                R_CORE_TRACE("Destroy event");
                EventDispatcher::Get()->Emit(ShutdownEvent());
                return 0;
            case WM_SIZE:
                uint32_t width = LOWORD(lParam);
                uint32_t height = HIWORD(lParam);

                R_CORE_TRACE("Resize event. New size: {0}x{1}", width, height);
                return 0;
        }

        return DefWindowProc(windowHandle, message, wParam, lParam);
    }
}