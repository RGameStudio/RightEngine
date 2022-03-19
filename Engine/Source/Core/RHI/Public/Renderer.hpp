#pragma once

#include "Shader.hpp"
#include "IndexBuffer.hpp"
#include "VertexArray.hpp"
#include "Window.hpp"
#include "Geometry.hpp"
#include "Config.hpp"

namespace RightEngine
{
    enum class LightNodeType;
    class LightNode;

    struct LightInfo
    {
        bool hasAmbient{ false };
        glm::vec3 ambientColor{ 1.0f, 1.0f, 1.0f };
        int pointLightAmount{ 0 };
        glm::vec3 pointLightPosition[MAX_POINT_LIGHTS];
        glm::vec3 pointLightColor[MAX_POINT_LIGHTS];
    };

    class Renderer
    {
    public:
        void SetWindow(Window* window);
        Window* GetWindow() const;

        void Draw(const std::shared_ptr<VertexArray>& va, const std::shared_ptr<IndexBuffer>& ib) const;
        void Draw(const std::shared_ptr<VertexArray>& va, const std::shared_ptr<VertexBuffer>& vb) const;
        void Draw(const std::shared_ptr<Geometry>& geometry) const;

        void Clear() const;

        void HasDepthTest(bool mode);

        void SetShader(const std::shared_ptr<Shader>& shader);
        const std::shared_ptr<Shader>& GetShader() const;

        void SetLight(const std::shared_ptr<LightNode>& node);
        void SaveLight() const;

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
        LightInfo lightInfo;
    };
}