#pragma once

#include "VulkanSwapchain.hpp"
#include "GraphicsPipeline.hpp"
#include <vulkan/vulkan.h>

namespace RightEngine
{
    class VulkanGraphicsPipeline : public GraphicsPipeline
    {
    public:
        VulkanGraphicsPipeline(const GraphicsPipelineDescriptor& descriptor,
                               const RenderPassDescriptor& renderPassDescriptor);

        ~VulkanGraphicsPipeline();

        VkRenderPass GetRenderPass() const
        { return renderPass; }

        VkPipeline GetPipeline() const
        { return graphicsPipeline; }

    private:
        VkPipeline graphicsPipeline;
        VkDescriptorSetLayout descriptorSetLayout;
        VkPipelineLayout pipelineLayout;
        VkRenderPass renderPass;
        VkDescriptorPool descriptorPool;
        std::vector<VkDescriptorSet> descriptorSets;

        void Init(const GraphicsPipelineDescriptor& descriptorconst,
                  const RenderPassDescriptor& renderPassDescriptor);
        void CreateRenderPass(const RenderPassDescriptor& renderPassDescriptor);
        void CreateDescriptorSetLayout();
        void CreateDescriptorPool();
        void CreateDescriptorSets();
    };
}