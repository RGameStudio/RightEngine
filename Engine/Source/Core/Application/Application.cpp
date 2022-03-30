#include "Application.hpp"
#include "Logger.hpp"
#include "DebugRHI.hpp"
#include "RendererCommand.hpp"
#include "Input.hpp"
#include "Types.hpp"
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
        RendererCommand::Init(GPU_API::OpenGL);
        DebugRHI::Init();

        R_CORE_INFO("Successfully initialized application!");
    }

    void Application::OnUpdate()
    {
        Input::OnUpdate();
        RendererCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
        RendererCommand::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
