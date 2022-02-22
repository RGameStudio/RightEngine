#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Shader.hpp"
#include "IndexBuffer.hpp"
#include "VertexArray.hpp"
#include "Window.hpp"
#include "Geometry.hpp"

namespace RightEngine
{
    class Renderer
    {
    public:
        void SetWindow(Window* window);
        Window* GetWindow() const;

        void Draw(const std::shared_ptr<VertexArray>& va, const std::shared_ptr<IndexBuffer>& ib) const;
        void Draw(const std::shared_ptr<VertexArray>& va, const std::shared_ptr<VertexBuffer>& vb) const;
        void Draw(const std::shared_ptr<Geometry>& geometry, const std::shared_ptr<Shader>& shader) const;
        void Clear() const;

        static Renderer& Get();
    private:
        Window* window = nullptr;
    };
}