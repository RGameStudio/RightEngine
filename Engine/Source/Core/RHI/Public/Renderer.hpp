#pragma once

#include "Window.hpp"
#include "Components.hpp"
#include "Config.hpp"
#include "Camera.hpp"
#include "Shader.hpp"
#include "Scene.hpp"
#include "Types.hpp"
#include <glm/matrix.hpp>

namespace RightEngine
{
    struct SceneData
    {
        glm::mat4 viewProjectionMatrix{ 1.0f };
    };

    enum class DepthTestMode
    {
        NEVER,
        LESS,
        EQUAL,
        LEQUAL,
        GREATER,
        NOTEQUAL,
        GEQUAL,
        ALWAYS
    };

    struct RendererSettings
    {
        bool hasDepthTest{ true };
        DepthTestMode depthTestMode{ DepthTestMode::LESS };
    };

    class Renderer
    {
    public:
        Renderer();

        void SubmitMesh(const std::shared_ptr<Shader>& shader,
                            const MeshComponent& mesh,
                            const glm::mat4& transform);

        void BeginFrame(const std::shared_ptr<Camera>& camera);
        void EndFrame();

        void UpdateBuffer(const std::shared_ptr<Buffer>& buffer, ShaderStage stage);
        void Draw(const std::shared_ptr<Buffer>& vertexBuffer, const std::shared_ptr<Buffer>& indexBuffer = nullptr);

        void SetPipeline(const std::shared_ptr<GraphicsPipeline>& aPipeline);

        void Configure();

        RendererSettings& GetSettings();

        static GPU_API GetAPI();

    private:
        SceneData sceneData;
        RendererSettings settings;
        std::shared_ptr<CommandBuffer> commandBuffer;
        std::shared_ptr<GraphicsPipeline> pipeline;
    };
}