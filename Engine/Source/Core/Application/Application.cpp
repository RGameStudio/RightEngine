#include <Logger.hpp>
#include "Application.hpp"
#include "../Window/WindowsWindow.hpp"

namespace RightEngine
{
    Application *Application::Get()
    {
        if (!instance)
        {
            instance = new Application();
        }

        return instance;
    }

    Application *Application::instance = nullptr;

    Application::Application()
    {
        Init();
    }

    void Application::Init()
    {
        WindowsWindow* _window = (WindowsWindow*) Window::Create("RightEngine2D", 800, 600);
        window.reset(_window);

        R_CORE_INFO("Successfully initialized application!");
    }

    void Application::OnUpdate()
    {
        window->OnUpdate();
    }

    void Application::OnUpdateEnd()
    {
        window->Swap();
    }
}