#pragma once

#include "IndexBuffer.hpp"
#include "VertexArray.hpp"
#include "Window.hpp"
#include "Geometry.hpp"
#include "Config.hpp"
#include "Camera.hpp"
#include "Shader.hpp"
#include <glm/matrix.hpp>

namespace RightEngine
{
    struct SceneData
    {
        glm::mat4 viewProjectionMatrix;
    };

    class Renderer
    {
    public:
        void SetWindow(Window* window);
        Window* GetWindow() const;

        void Draw(const std::shared_ptr<VertexArray>& va, const std::shared_ptr<IndexBuffer>& ib) const;
        void Draw(const std::shared_ptr<VertexArray>& va, const std::shared_ptr<VertexBuffer>& vb) const;
        void Draw(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Geometry>& geometry, const glm::mat4& transform) const;

        virtual void BeginScene(const std::shared_ptr<Scene>& scene);
        virtual void EndScene();

        void SetClearColor(const glm::vec4& color) const;
        void Clear(uint32_t clearBits) const;

        void HasDepthTest(bool mode);

        static Renderer& Get();

        Renderer() = default;
        ~Renderer() = default;
        Renderer(const Renderer& other) = delete;
        Renderer& operator=(const Renderer& other) = delete;
        Renderer(Renderer&& other) = delete;
        Renderer& operator=(Renderer&& other) = delete;

    private:
        Window* window{ nullptr };
        SceneData sceneData;
    };
}