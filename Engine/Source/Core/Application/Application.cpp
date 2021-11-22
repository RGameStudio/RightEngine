#include "Application.hpp"
#include <memory>
#include <Logger.hpp>
#include <DebugRHI.hpp>
#include "../Window/WindowsWindow.hpp"
#include "Renderer.hpp"
#include "Shader.hpp"
#include "Input.hpp"

namespace RightEngine
{
    Application *Application::Get()
    {
        if (!instance)
        {
            startTimestamp = high_resolution_clock::now();
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
        DebugRHI::Init();

        R_CORE_INFO("Successfully initialized application!");

        float quadVertices[] = {
                -0.5f, -0.5f, 0.0f,
                0.5f, -0.5f, 0.0f,
                0.5f, 0.5f, 0.0f,
                -0.5f, 0.5f, 0.0f,
        };

        uint32_t quadIndexes[] = {
                0, 1, 2,
                2, 3, 0
        };

        VertexBufferLayout layout;
        layout.Push<float>(3);

        basicShader = std::make_unique<Shader>("Resources/Shaders/Basic/basic.vert", "Resources/Shaders/Basic/basic.frag");
        testQuadVertexBuffer = std::make_unique<VertexBuffer>(quadVertices, sizeof(quadVertices));
        testQuadIndexBuffer = std::make_unique<IndexBuffer>(quadIndexes, sizeof(quadIndexes) / sizeof(uint32_t));
        testQuadVertexArray = std::make_unique<VertexArray>();
        testQuadVertexArray->AddBuffer(*testQuadVertexBuffer, layout);
    }

    void Application::OnUpdate()
    {
        Input::OnUpdate();
        window->OnUpdate();
        Renderer::Draw(*testQuadVertexArray, *testQuadIndexBuffer, *basicShader);
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
}