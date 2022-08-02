#pragma once

#include "VulkanSwapchain.hpp"
#include "Shader.hpp"
#include <vulkan/vulkan.h>
#include <glm/vec2.hpp>

namespace RightEngine
{
    struct GraphicsPipelineDescriptor
    {
        glm::ivec2 extent;
        std::shared_ptr<Shader> shader;
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

    private:
        VkPipeline graphicsPipeline;
        VkPipelineLayout pipelineLayout;
        VkRenderPass renderPass;
        GraphicsPipelineDescriptor pipelineDescriptor;
        RenderPassDescriptor renderPassDescriptor;

        void Init(const GraphicsPipelineDescriptor& descriptorconst,
                  const RenderPassDescriptor& renderPassDescriptor);
        void CreateRenderPass(const RenderPassDescriptor& renderPassDescriptor);
    };
}