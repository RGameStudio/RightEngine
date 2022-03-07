#include "Application.hpp"
#include "Logger.hpp"
#include "DebugRHI.hpp"
#include "Renderer.hpp"
#include "Input.hpp"
#include <memory>

namespace RightEngine
{
    Application& Application::Get()
    {
        static Application application;
        return application;
    }

    Application::Application()
    {
        Init();
    }

    void Application::Init()
    {
        Window* _window = Window::Create("Engine window", 1920, 1080);
        window.reset(_window);
        auto& renderer = Renderer::Get();
        renderer.SetWindow(_window);
        DebugRHI::Init();

        R_CORE_INFO("Successfully initialized application!");
    }

    void Application::OnUpdate()
    {
        Input::OnUpdate();
        Renderer::Get().Clear();
        window->OnUpdate();
        activeScene->OnUpdate();
    }

    void Application::OnUpdateEnd()
    {
        window->Swap();
    }

    double Application::GetTime()
    {
        using namespace std::chrono;
        return duration_cast<milliseconds>(std::chrono::high_resolution_clock::now() - startTimestamp).count();
    }
    time_point<high_resolution_clock> Application::startTimestamp;

    void Application::SetScene(const std::shared_ptr<Scene>& scene)
    {
        activeScene = scene;
    }
}