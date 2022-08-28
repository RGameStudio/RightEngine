#include "VulkanRendererState.hpp"
#include "VulkanGraphicsPipeline.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanTexture.hpp"
#include "VulkanSampler.hpp"
#include "Shader.hpp"

using namespace RightEngine;

void VulkanRendererState::OnUpdate(const std::shared_ptr<GraphicsPipeline>& pipeline)
{
    if (descriptorSet)
    {
        if (isSyncNeeded)
        {
            std::vector<VkWriteDescriptorSet> writeDescriptorSets;
            std::vector<VkDescriptorBufferInfo> bufferInfos;
            std::vector<VkDescriptorImageInfo> textureInfos;
            for (const auto& [bufferRef, bufferPtr] : buffersToSync)
            {
                if (!bufferPtr.expired())
                {
                    const auto buffer = bufferPtr.lock();
                    VkDescriptorBufferInfo bufferInfo{};
                    bufferInfo.buffer = std::static_pointer_cast<VulkanBuffer>(buffer)->GetBuffer();
                    bufferInfo.offset = 0;
                    bufferInfo.range = buffer->GetDescriptor().size;

                    bufferInfos.push_back(bufferInfo);

                    VkWriteDescriptorSet descriptorWrite{};
                    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    descriptorWrite.dstSet = descriptorSet;
                    descriptorWrite.dstBinding = bufferRef.slot;
                    descriptorWrite.dstArrayElement = 0;
                    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    descriptorWrite.descriptorCount = 1;
                    descriptorWrite.pBufferInfo = &bufferInfos.back();

                    writeDescriptorSets.push_back(descriptorWrite);
                }
            }

            for (const auto& [slot, texturePtr] : texturesToSync)
            {
                if (!texturePtr.expired())
                {
                    const auto texture = std::static_pointer_cast<VulkanTexture>(texturePtr.lock());
                    VkDescriptorImageInfo imageInfo{};
                    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    imageInfo.imageView = texture->GetImageView();
                    imageInfo.sampler = std::static_pointer_cast<VulkanSampler>(texture->GetSampler())->GetSampler();

                    textureInfos.push_back(imageInfo);

                    VkWriteDescriptorSet descriptorWrite{};
                    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    descriptorWrite.dstSet = descriptorSet;
                    descriptorWrite.dstBinding = slot;
                    descriptorWrite.dstArrayElement = 0;
                    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    descriptorWrite.descriptorCount = 1;
                    descriptorWrite.pImageInfo = &textureInfos.back();

                    writeDescriptorSets.push_back(descriptorWrite);
                }
            }

            vkUpdateDescriptorSets(VK_DEVICE()->GetDevice(),
                                   writeDescriptorSets.size(),
                                   writeDescriptorSets.data(), 0,
                                   nullptr);
        }
    }
    else
    {
        const auto& shaderDescriptor = pipeline->GetPipelineDescriptor().shader->GetShaderProgramDescriptor();

        VkDescriptorPoolSize bufferPoolSize{};
        bufferPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        bufferPoolSize.descriptorCount = shaderDescriptor.reflection.buffers.size();

        VkDescriptorPoolSize texturePoolSize{};
        bufferPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        bufferPoolSize.descriptorCount = shaderDescriptor.reflection.textures.size();

        std::array<VkDescriptorPoolSize, 2> poolSizes = { bufferPoolSize, texturePoolSize };

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = poolSizes.size();
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = 1;

        if (vkCreateDescriptorPool(VK_DEVICE()->GetDevice(), &poolInfo, nullptr,
                                   &descriptorPool) != VK_SUCCESS)
        {
            R_CORE_ASSERT(false, "failed to create descriptor pool!");
        }

        std::vector<VkDescriptorSetLayout> layouts(1,std::static_pointer_cast<VulkanGraphicsPipeline>(
                                                           pipeline)->GetDescriptorSetLayout());
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = layouts.data();

        if (vkAllocateDescriptorSets(VK_DEVICE()->GetDevice(), &allocInfo, &descriptorSet) != VK_SUCCESS)
        {
            R_CORE_ASSERT(false, "failed to allocate descriptor sets!");
        }

        OnUpdate(pipeline);
    }
}

VulkanRendererState::~VulkanRendererState()
{
    if (descriptorPool)
    {
        vkDestroyDescriptorPool(VK_DEVICE()->GetDevice(), descriptorPool, nullptr);
    }
}

