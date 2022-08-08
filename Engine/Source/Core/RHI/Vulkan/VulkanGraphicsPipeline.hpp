#pragma once

#include "VulkanSwapchain.hpp"
#include "Shader.hpp"
#include "Buffer.hpp"
#include <vulkan/vulkan.h>
#include <glm/vec2.hpp>
#include <unordered_map>

namespace RightEngine
{
    struct GraphicsPipelineDescriptor
    {
        glm::ivec2 extent;
        std::shared_ptr<Shader> shader;
        std::unordered_map<int, std::shared_ptr<Buffer>> vertexBuffers;
        std::unordered_map<int, std::shared_ptr<Buffer>> buffers;
    };

    struct RenderPassDescriptor
    {
        Format format;
    };

    class VulkanGraphicsPipeline
    {
    public:
        VulkanGraphicsPipeline(const GraphicsPipelineDescriptor& descriptor,
                               const RenderPassDescriptor& renderPassDescriptor);

        ~VulkanGraphicsPipeline();

        VkRenderPass GetRenderPass() const
        { return renderPass; }

        VkPipeline GetPipeline() const
        { return graphicsPipeline; }

        const GraphicsPipelineDescriptor& GetPipelineDescriptor() const
        { return pipelineDescriptor; }

        const RenderPassDescriptor& GetRenderPassDescriptor() const
        { return renderPassDescriptor; }

    private:
        VkPipeline graphicsPipeline;
        VkDescriptorSetLayout descriptorSetLayout;
        VkPipelineLayout pipelineLayout;
        VkRenderPass renderPass;
        VkDescriptorPool descriptorPool;
        std::vector<VkDescriptorSet> descriptorSets;
        GraphicsPipelineDescriptor pipelineDescriptor;
        RenderPassDescriptor renderPassDescriptor;

        void Init(const GraphicsPipelineDescriptor& descriptorconst,
                  const RenderPassDescriptor& renderPassDescriptor);
        void CreateRenderPass(const RenderPassDescriptor& renderPassDescriptor);
        void CreateDescriptorSetLayout();
        void CreateDescriptorPool();
        void CreateDescriptorSets();
    };
}