#pragma once

#include "RendererAPI.hpp"
#include "VulkanRenderingContext.hpp"
#include "VulkanSurface.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanGraphicsPipeline.hpp"
#include "VulkanCommandBuffer.hpp"

namespace RightEngine
{
    class VulkanRendererAPI : public RendererAPI
    {
    public:
        virtual void Init() override;

        virtual void BeginFrame(const std::shared_ptr<CommandBuffer>& cmd) override;
        virtual void EndFrame(const std::shared_ptr<CommandBuffer>& cmd) override;

        virtual void Configure(const RendererSettings& settings) override;

        virtual void SetClearColor(const glm::vec4& color) override;

        virtual void Clear(uint32_t clearBits) override;

        virtual void SetViewport(const Viewport& viewport) override;
        virtual Viewport GetViewport() override;

        virtual void Draw(const std::shared_ptr<CommandBuffer>& cmd,
                          const std::shared_ptr<Buffer>& vertexBuffer,
                          const std::shared_ptr<Buffer>& indexBuffer) override;
        virtual void Draw(const std::shared_ptr<CommandBuffer>& cmd,
                          const std::shared_ptr<Buffer>& buffer) override;
        virtual ~VulkanRendererAPI() override;

    private:
        std::shared_ptr<VulkanRenderingContext> context;
        std::shared_ptr<VulkanSurface> surface;
        std::shared_ptr<VulkanSwapchain> swapchain;
        std::shared_ptr<VulkanGraphicsPipeline> pipeline;
        std::vector<VkFramebuffer> swapchainFramebuffers;
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        uint32_t currentImageIndex;

        void RecordCommandBuffer(const std::shared_ptr<VulkanCommandBuffer>& cmd, uint32_t imageIndex);
        void CreateSyncObjects();
        void CreateFramebuffers();
        void CreateSwapchain();
        void CreateDescriptorSetLayout();
        void DestroySwapchain();
    };
}