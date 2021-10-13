#pragma once

#include <memory>
#include <IndexBuffer.hpp>
#include <VertexBuffer.hpp>
#include <VertexArray.hpp>
#include "Window.hpp"

namespace RightEngine
{
    class Application
    {
    public:
        static Application *Get();

        void OnUpdate();

        void OnUpdateEnd();

    private:
        static Application *instance;

        Application();

        void Init();

        std::unique_ptr<Window> window;
        std::unique_ptr<IndexBuffer> testQuadIndexBuffer;
        std::unique_ptr<VertexBuffer> testQuadVertexBuffer;
        std::unique_ptr<VertexArray> testQuadVertexArray;
    };

}