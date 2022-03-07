#pragma once

#include "IndexBuffer.hpp"
#include "VertexBuffer.hpp"
#include "VertexArray.hpp"
#include "Shader.hpp"
#include "Window.hpp"
#include "Camera.hpp"
#include "Geometry.hpp"
#include "Scene.hpp"
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

        static double GetTime();

        void SetScene(const std::shared_ptr<Scene>& scene);

        Application(const Application& other) = delete;
        Application& operator=(const Application& other) = delete;
        Application(Application&& other) = delete;
        Application& operator=(Application&& other) = delete;

    private:
        static time_point<high_resolution_clock> startTimestamp;

        Application();

        void Init();

        std::unique_ptr<Window> window;
        std::shared_ptr<Scene> activeScene;
    };

}