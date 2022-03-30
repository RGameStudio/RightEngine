#pragma once

#include "IndexBuffer.hpp"
#include "VertexBuffer.hpp"
#include "Types.hpp"
#include <glm/glm.hpp>
#include <memory>

namespace RightEngine
{
    struct RendererSettings;

    class RendererAPI
    {
    public:
        virtual void Init() = 0;

        virtual void Configure(const RendererSettings& settings) = 0;

        virtual void SetClearColor(const glm::vec4& color) = 0;

        virtual void Clear(uint32_t clearBits) = 0;

        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

        virtual void DrawIndexed(const std::shared_ptr<IndexBuffer>& ib) = 0;

        virtual void Draw(const std::shared_ptr<VertexBuffer>& vb) = 0;

        static std::shared_ptr<RendererAPI> Create(GPU_API api);
    };
}
