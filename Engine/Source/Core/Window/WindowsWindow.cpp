#include <glad/glad.h>
#include <RenderingContext.hpp>
#include "WindowsWindow.hpp"
#include "Core.h"

namespace RightEngine
{
    LRESULT CALLBACK WndProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

    WindowsWindow::WindowsWindow(std::string title, uint32_t width, uint32_t height) : Window(title, width, height)
    {
        appInstance = GetModuleHandle(nullptr);

        WNDCLASSEX wc;
        ZeroMemory(&wc, sizeof(WNDCLASSEX));

        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wc.lpfnWndProc = WndProc;
        wc.hInstance = appInstance;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = NULL;
        wc.lpszClassName = "EngineWindowClass";

        R_CORE_ASSERT(RegisterClassEx(&wc), "Can't register window class");

        RECT rect;
        ZeroMemory(&rect, sizeof(RECT));
        rect.right = width;
        rect.bottom = height;

        DWORD windowStyle = WS_OVERLAPPEDWINDOW;

        AdjustWindowRectEx(&rect, windowStyle, false, NULL);

        windowHandle = CreateWindowEx(
                0x0,                  // Optional window styles.
                wc.lpszClassName,     // Window class
                "RightEngine2D",      // Window text
                windowStyle,          // Window style
                // Size and position
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                rect.right - rect.left,
                rect.bottom - rect.top,
                ////////////////////
                nullptr,              // Parent window
                nullptr,              // Menu
                appInstance,          // Instance handle
                nullptr               // Additional application data
        );

        R_CORE_ASSERT(windowHandle, "Can't create engine window!");

        RenderingContext* context = RenderingContext::Get(GetDC(windowHandle));

        ShowWindow(windowHandle, SW_SHOWDEFAULT);

        R_CORE_INFO("Created window successfully! Size: {0}x{1}", width, height);
    }

    void WindowsWindow::OnUpdate()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(117.0f / 256.0f, 170.0f / 256.0f, 1.0f, 1.0f);
        MSG message;
        while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
    }

    void WindowsWindow::Swap()
    {
        SwapBuffers(GetDC(windowHandle));
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
                //TODO Proper window resizing
                uint32_t width = LOWORD(lParam);
                uint32_t height = HIWORD(lParam);

                RECT rect;
                ZeroMemory(&rect, sizeof(RECT));
                rect.right = width;
                rect.bottom = height;

                AdjustWindowRectEx(&rect, WS_OVERLAPPED, false, NULL);
                R_CORE_TRACE("Resize event. New size: {0}x{1}", width, height);
                return 0;
        }

        return DefWindowProc(windowHandle, message, wParam, lParam);
    }
}