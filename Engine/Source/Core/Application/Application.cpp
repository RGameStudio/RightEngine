#include "Application.hpp"
#include "Logger.hpp"
#include "DebugRHI.hpp"
#include "Renderer.hpp"
#include "Shader.hpp"
#include "Input.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <memory>

namespace RightEngine
{
    Application* Application::Get()
    {
        if (!instance)
        {
            startTimestamp = high_resolution_clock::now();
            instance = new Application();
        }

        return instance;
    }

    Application* Application::instance = nullptr;

    Application::Application()
    {
        Init();
    }

    void Application::Init()
    {
        Window* _window = Window::Create("Engine window", 1920, 1080);
        window.reset(_window);
        auto renderer = Renderer::Get();
        renderer.SetWindow(_window);
        DebugRHI::Init();

        R_CORE_INFO("Successfully initialized application!");

//        float quadVertices[] = {
//                -0.5f, -0.5f, 0.0f,
//                0.5f, -0.5f, 0.0f,
//                0.5f, 0.5f, 0.0f,
//                -0.5f, 0.5f, 0.0f,
//        };
//
//        uint32_t quadIndexes[] = {
//                0, 1, 2,
//                2, 3, 0
//        };

        static const GLfloat g_vertex_buffer_data[] = {
                -1.0f, -1.0f, -1.0f, // triangle 1 : begin
                -1.0f, -1.0f, 1.0f,
                -1.0f, 1.0f, 1.0f, // triangle 1 : end
                1.0f, 1.0f, -1.0f, // triangle 2 : begin
                -1.0f, -1.0f, -1.0f,
                -1.0f, 1.0f, -1.0f, // triangle 2 : end
                1.0f, -1.0f, 1.0f,
                -1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, 1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f, -1.0f,
                -1.0f, 1.0f, 1.0f,
                -1.0f, 1.0f, -1.0f,
                1.0f, -1.0f, 1.0f,
                -1.0f, -1.0f, 1.0f,
                -1.0f, -1.0f, -1.0f,
                -1.0f, 1.0f, 1.0f,
                -1.0f, -1.0f, 1.0f,
                1.0f, -1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, 1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, -1.0f,
                -1.0f, 1.0f, -1.0f,
                1.0f, 1.0f, 1.0f,
                -1.0f, 1.0f, -1.0f,
                -1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                -1.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 1.0f
        };

        VertexBufferLayout layout;
        layout.Push<float>(3);

        camera = std::make_shared<FPSCamera>(glm::vec3(0, 5, -15), glm::vec3(0, 1, 0));
        testGeometry = std::make_shared<Geometry>();
        testGeometry->CreateVertexBuffer(g_vertex_buffer_data, sizeof(g_vertex_buffer_data));
        testGeometry->CreateVertexArray(layout);
        testShader = std::make_shared<Shader>("/Assets/Shaders/Basic/basic.vert", "/Assets/Shaders/Basic/basic.frag");
    }

    void Application::OnUpdate()
    {
        Input::OnUpdate();
        Renderer::Get().Clear();
        window->OnUpdate();
        camera->OnUpdate();
        testShader->Bind();
        const glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f),
                                                            static_cast<float>(window->GetWidth()) /
                                                            static_cast<float>(window->GetHeight()), 0.1f, 300.0f);
        testShader->SetUniformMat4f("projection", projectionMatrix);
        const auto view = camera->GetViewMatrix();
        testShader->SetUniformMat4f("view", view);
        glm::mat4 model(1);
        model = glm::translate(model, glm::vec3(0, 0, 0));
        model = glm::scale(model, glm::vec3(1, 1, 1));
        model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0, 1, 0));
        testShader->SetUniformMat4f("model", model);
        Renderer::Get().Draw(testGeometry, testShader);
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