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

    Application::~Application()
    {
        for (auto& layer: layers)
        {
            layer->OnDetach();
            layer.reset();
        }

        layers.clear();
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

        for (const auto& layer: layers)
        {
            layer->OnUpdate(Input::deltaTime);
        }

        // TODO: ImGUI layer
    }

    void Application::OnUpdateEnd()
    {
        window->Swap();
    }

    void Application::PushLayer(const std::shared_ptr<Layer>& layer)
    {
        layers.emplace_back(layer);
        layer->OnAttach();
    }
}
