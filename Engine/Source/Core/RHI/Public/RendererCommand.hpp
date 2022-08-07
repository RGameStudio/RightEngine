#pragma once

#include "Types.hpp"
#include "Shader.hpp"
#include "RendererAPI.hpp"

namespace RightEngine
{
    class RendererCommand
    {
    public:
        static void Init(GPU_API api);
        static void BeginFrame();
        static void EndFrame();

        static void SetClearColor(const glm::vec4& color);

        static void Clear(uint32_t clearBits);

        static void SetViewport(const Viewport& viewport);
        static Viewport GetViewport();

        static void Draw(const std::shared_ptr<Buffer>& buffer);
        static void DrawIndexed(const std::shared_ptr<Buffer>& vertexBuffer, const std::shared_ptr<Buffer>& indexBuffer);

        static void Configure(const RendererSettings& settings);

    private:
        static std::shared_ptr<RendererAPI> rendererAPI;
    };
}
