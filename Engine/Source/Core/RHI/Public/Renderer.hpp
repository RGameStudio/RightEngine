#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "Shader.hpp"
#include "IndexBuffer.hpp"
#include "VertexArray.hpp"
#include "Window.hpp"

namespace RightEngine
{
    class Renderer
    {
    public:
        void SetWindow(Window* window);
        Window* GetWindow() const;

        void Draw(const VertexArray& va, const IndexBuffer& ib);
        void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader);
        void Draw(const VertexArray& va, const VertexBuffer& vb);
        void Clear() const;

        static Renderer& Get();
    private:
        Window* window = nullptr;
    };
}