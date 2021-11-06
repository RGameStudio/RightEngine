#pragma once

#include <memory>
#include <chrono>
#include <IndexBuffer.hpp>
#include <VertexBuffer.hpp>
#include <VertexArray.hpp>
#include <Shader.hpp>
#include "Window.hpp"

namespace RightEngine
{
    using namespace std::chrono;

    class Application
    {
    public:
        static Application *Get();

        void OnUpdate();

        void OnUpdateEnd();

        static double GetTime();

    private:
        static Application *instance;
        static time_point<high_resolution_clock> startTimestamp;

        Application();

        void Init();

        std::unique_ptr<Window> window;
        std::unique_ptr<IndexBuffer> testQuadIndexBuffer;
        std::unique_ptr<VertexBuffer> testQuadVertexBuffer;
        std::unique_ptr<VertexArray> testQuadVertexArray;
        std::unique_ptr<Shader> basicShader;
    };

}