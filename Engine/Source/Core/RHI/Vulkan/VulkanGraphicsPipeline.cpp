#include "VulkanGraphicsPipeline.hpp"
#include "VulkanConverters.hpp"
#include "VulkanShader.hpp"
#include "Assert.hpp"
#include "VulkanBuffer.hpp"
#include <vector>

using namespace RightEngine;

namespace
{
    std::vector<VkWriteDescriptorSet> GetWriteDescriptorSets(const std::vector<VkDescriptorSet>& descriptorSets,
                                                             std::unordered_map<int, std::shared_ptr<Buffer>>& buffers)
    {
        std::vector<VkWriteDescriptorSet> writeDescriptorSet;
        for (const auto& [slot, buffer] : buffers)
        {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = std::static_pointer_cast<VulkanBuffer>(buffer)->GetBuffer();
            bufferInfo.offset = 0;
            bufferInfo.range = buffer->GetDescriptor().size;

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = descriptorSets[0];
            descriptorWrite.dstBinding = slot;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &bufferInfo;

            writeDescriptorSet.emplace_back(descriptorWrite);
        }

        return writeDescriptorSet;
    }
}

VulkanGraphicsPipeline::VulkanGraphicsPipeline(const GraphicsPipelineDescriptor& descriptor,
                                               const RenderPassDescriptor& renderPassDescriptor) : GraphicsPipeline(descriptor, renderPassDescriptor)
{
    Init(descriptor, renderPassDescriptor);
}

void VulkanGraphicsPipeline::Init(const GraphicsPipelineDescriptor& descriptor,
                                  const RenderPassDescriptor& renderPassDescriptor)
{
    std::vector<VkDynamicState> dynamicStates =
            {
                    VK_DYNAMIC_STATE_VIEWPORT,
                    VK_DYNAMIC_STATE_SCISSOR
            };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    const auto shader = std::static_pointer_cast<VulkanShader>(descriptor.shader);
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    VkVertexInputBindingDescription bindingDescription[] = { shader->GetVertexFormatDescription() };
    vertexInputInfo.pVertexBindingDescriptions = bindingDescription;
    const auto vertexFormatAttribute = shader->GetVertexFormatAttribute();
    vertexInputInfo.vertexAttributeDescriptionCount = vertexFormatAttribute.size();
    vertexInputInfo.pVertexAttributeDescriptions = vertexFormatAttribute.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) descriptor.extent.x;
    viewport.height = (float) descriptor.extent.y;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = VulkanConverters::Extent(descriptor.extent);

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = std::static_pointer_cast<VulkanShader>(descriptor.shader)->GetShaderModule(ShaderModuleType::VERTEX);
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = std::static_pointer_cast<VulkanShader>(descriptor.shader)->GetShaderModule(ShaderModuleType::FRAGMENT);
    fragShaderStageInfo.pName = "main";

    CreateDescriptorSetLayout();
    CreateDescriptorPool();
    CreateDescriptorSets();

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages{ vertShaderStageInfo, fragShaderStageInfo };


    if (vkCreatePipelineLayout(VK_DEVICE()->GetDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
    {
        R_CORE_ASSERT(false, "failed to create pipeline layout!");
    }

    CreateRenderPass(renderPassDescriptor);

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = shaderStages.size();
    pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr; // Optional
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1; // Optional

    if (vkCreateGraphicsPipelines(VK_DEVICE()->GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
    {
        R_CORE_ASSERT(false, "failed to create graphics pipeline!");
    }
}

void VulkanGraphicsPipeline::CreateRenderPass(const RenderPassDescriptor& renderPassDescriptor)
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = VulkanConverters::Format(renderPassDescriptor.format);
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(VK_DEVICE()->GetDevice(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
    {
        R_CORE_ASSERT(false, "failed to create render pass!");
    }
}

void VulkanGraphicsPipeline::CreateDescriptorSetLayout()
{
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    for (const auto& [slot, buffer] : pipelineDescriptor.vertexBuffers)
    {
        VkDescriptorSetLayoutBinding vertexUboLayoutBinding{};
        vertexUboLayoutBinding.binding = slot;
        vertexUboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        vertexUboLayoutBinding.descriptorCount = 1;
        vertexUboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        vertexUboLayoutBinding.pImmutableSamplers = nullptr; // Optional
        bindings.emplace_back(vertexUboLayoutBinding);
    }

    for (const auto& [slot, buffer] : pipelineDescriptor.buffers)
    {
        VkDescriptorSetLayoutBinding vertexUboLayoutBinding{};
        vertexUboLayoutBinding.binding = slot;
        vertexUboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        vertexUboLayoutBinding.descriptorCount = 1;
        vertexUboLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        vertexUboLayoutBinding.pImmutableSamplers = nullptr; // Optional
        bindings.emplace_back(vertexUboLayoutBinding);
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = bindings.size();
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(VK_DEVICE()->GetDevice(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void VulkanGraphicsPipeline::CreateDescriptorPool()
{
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = pipelineDescriptor.vertexBuffers.size() + pipelineDescriptor.buffers.size();

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = 1;

    if (vkCreateDescriptorPool(VK_DEVICE()->GetDevice(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void VulkanGraphicsPipeline::CreateDescriptorSets()
{
    std::vector<VkDescriptorSetLayout> layouts(1, descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(1);
    if (vkAllocateDescriptorSets(VK_DEVICE()->GetDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    auto vertexWriteDescSets = GetWriteDescriptorSets(descriptorSets, pipelineDescriptor.vertexBuffers);
    const auto fragWriteDescSets = GetWriteDescriptorSets(descriptorSets, pipelineDescriptor.buffers);
    vertexWriteDescSets.insert(vertexWriteDescSets.end(), fragWriteDescSets.begin(), fragWriteDescSets.end());

    vkUpdateDescriptorSets(VK_DEVICE()->GetDevice(), 1, vertexWriteDescSets.data(), 0, nullptr);
}

VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
{
    vkDestroyPipeline(VK_DEVICE()->GetDevice(), graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(VK_DEVICE()->GetDevice(), pipelineLayout, nullptr);
    vkDestroyRenderPass(VK_DEVICE()->GetDevice(), renderPass, nullptr);
    vkDestroyDescriptorPool(VK_DEVICE()->GetDevice(), descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(VK_DEVICE()->GetDevice(), descriptorSetLayout, nullptr);
}