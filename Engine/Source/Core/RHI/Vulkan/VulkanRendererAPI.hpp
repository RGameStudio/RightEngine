#pragma once

#include "RendererAPI.hpp"

namespace RightEngine
{
    class VulkanRendererAPI : public RendererAPI
    {
    public:
        virtual void Init() override;

        virtual void Configure(const RendererSettings& settings) override;

        virtual void SetClearColor(const glm::vec4& color) override;

        virtual void Clear(uint32_t clearBits) override;

        virtual void SetViewport(const Viewport& viewport) override;
        virtual Viewport GetViewport() override;

        virtual void DrawIndexed(const std::shared_ptr<IndexBuffer>& ib) override;
        virtual void Draw(const std::shared_ptr<VertexBuffer>& vb) override;
        virtual ~VulkanRendererAPI() override;
    };
}