#pragma once

#include "Shader.hpp"
#include "Window.hpp"
#include "Camera.hpp"
#include "Scene.hpp"
#include "Layer.hpp"
#include "ImGuiLayer.hpp"
#include "RenderingContext.hpp"
#include <memory>
#include <chrono>

namespace RightEngine
{
    using namespace std::chrono;

    class Application
    {
    public:
        static Application& Get();

        void OnUpdate();
        void OnUpdateEnd();

        // TODO: Delete layers
        void PushLayer(const std::shared_ptr<Layer>& layer);
        void PushOverlay(const std::shared_ptr<Layer>& layer);

        const std::shared_ptr<Window>& GetWindow() const;

        void Init();

        Application(const Application& other) = delete;
        Application& operator=(const Application& other) = delete;
        Application(Application&& other) = delete;
        Application& operator=(Application&& other) = delete;

    private:
        std::shared_ptr<Window> window;
        std::vector<std::shared_ptr<Layer>> layers;

    private:
        Application();
        ~Application();
    };

}