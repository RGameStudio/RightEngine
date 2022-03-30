#pragma once

#include "Types.hpp"
#include "Shader.hpp"
#include "Geometry.hpp"
#include "RendererAPI.hpp"

namespace RightEngine
{
    class RendererCommand
    {
    public:
        static void Init(GPU_API api);

        static void SetClearColor(const glm::vec4& color);

        static void Clear(uint32_t clearBits);

        static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

        static void DrawIndexed(const std::shared_ptr<IndexBuffer>& ib);

        static void Draw(const std::shared_ptr<VertexBuffer>& vb);

        static void Configure(const RendererSettings& settings);

    private:
        static std::shared_ptr<RendererAPI> rendererAPI;
    };
}
