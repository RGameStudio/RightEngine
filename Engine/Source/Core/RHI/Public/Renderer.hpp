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
        void Draw(const std::shared_ptr<Geometry>& geometry) const;

        void Clear() const;

        void SetShader(const std::shared_ptr<Shader>& shader);
        const std::shared_ptr<Shader>& GetShader() const;

        static Renderer& Get();

        Renderer() = default;
        ~Renderer() = default;
        Renderer(const Renderer& other) = delete;
        Renderer& operator=(const Renderer& other) = delete;
        Renderer(Renderer&& other) = delete;
        Renderer& operator=(Renderer&& other) = delete;

    private:
        Window* window{ nullptr };
        std::shared_ptr<Shader> shader;
    };
}