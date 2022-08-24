#include "VulkanTexture.hpp"
#include "VulkanConverters.hpp"
#include "VulkanUtils.hpp"
#include "VulkanBuffer.hpp"
#include "Buffer.hpp"

using namespace RightEngine;

namespace
{
    void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
    {
        CommandBufferDescriptor commandBufferDescriptor;
        commandBufferDescriptor.type = CommandBufferType::GRAPHICS;
        auto commandBuffer = std::static_pointer_cast<VulkanCommandBuffer>(Device::Get()->CreateCommandBuffer(commandBufferDescriptor));

        VulkanUtils::BeginCommandBuffer(commandBuffer, true);

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED
        && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
        && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else
        {
            throw std::invalid_argument("unsupported layout transition!");
        }

        commandBuffer->Enqueue([=](auto buffer)
        {
            vkCmdPipelineBarrier(
                    VK_CMD(buffer)->GetBuffer(),
                    sourceStage, destinationStage,
                    0,
                    0, nullptr,
                    0, nullptr,
                    1, &barrier
            );
        });

        VulkanUtils::EndCommandBuffer(VK_DEVICE(), commandBuffer);
    }

    void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
        CommandBufferDescriptor commandBufferDescriptor;
        commandBufferDescriptor.type = CommandBufferType::GRAPHICS;
        auto commandBuffer = std::static_pointer_cast<VulkanCommandBuffer>(Device::Get()->CreateCommandBuffer(commandBufferDescriptor));

        VulkanUtils::BeginCommandBuffer(commandBuffer, true);

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;

        region.imageOffset = {0, 0, 0};
        region.imageExtent = { width, height,1 };

        commandBuffer->Enqueue([=](auto cmdBuffer)
                               {
                                   vkCmdCopyBufferToImage(
                                           VK_CMD(cmdBuffer)->GetBuffer(),
                                           buffer,
                                           image,
                                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                           1,
                                           &region
                                   );
                               });

        VulkanUtils::EndCommandBuffer(VK_DEVICE(), commandBuffer);
    }
}

VulkanTexture::VulkanTexture(const std::shared_ptr<Device>& device,
                             const TextureDescriptor& aSpecification,
                             const std::vector<uint8_t>& data) : Texture(device, aSpecification, data)
{
    const auto vkDevice = std::static_pointer_cast<VulkanDevice>(device);
    Init(vkDevice, data);
}

void VulkanTexture::Init(const std::shared_ptr<VulkanDevice>& device,
                         const std::vector<uint8_t>& data)
{
    BufferDescriptor stagingBufferDesc;
    stagingBufferDesc.size = specification.GetTextureSize();
    stagingBufferDesc.type = BUFFER_TYPE_TRANSFER_SRC;
    stagingBufferDesc.memoryType = static_cast<MemoryType>(MEMORY_TYPE_HOST_COHERENT | MEMORY_TYPE_HOST_VISIBLE);
    stagingBuffer = device->CreateBuffer(stagingBufferDesc, nullptr);
    auto ptr = stagingBuffer->Map();
    memcpy(ptr, data.data(), stagingBufferDesc.size);
    stagingBuffer->UnMap();

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VulkanConverters::ImageType(specification.type);
    imageInfo.extent.width = specification.width;
    imageInfo.extent.height = specification.height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.flags = 0; // Optional

    if (vkCreateImage(device->GetDevice(), &imageInfo, nullptr, &textureImage) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device->GetDevice(), textureImage, &memRequirements);

    textureImageMemory = VulkanUtils::AllocateMemory(memRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vkBindImageMemory(device->GetDevice(), textureImage, textureImageMemory, 0);

    TransitionImageLayout(textureImage,
                          VK_FORMAT_R8G8B8A8_SRGB,
                          VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    CopyBufferToImage(std::static_pointer_cast<VulkanBuffer>(stagingBuffer)->GetBuffer(),
                      textureImage,
                      specification.width,
                      specification.height);

    TransitionImageLayout(textureImage,
                          VK_FORMAT_R8G8B8A8_SRGB,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    stagingBuffer.reset();
}


void VulkanTexture::GenerateMipmaps() const
{

}

void VulkanTexture::Bind(uint32_t slot) const
{

}

void VulkanTexture::UnBind() const
{

}

VulkanTexture::~VulkanTexture()
{
    vkDestroyImage(VK_DEVICE()->GetDevice(), textureImage, nullptr);
    vkFreeMemory(VK_DEVICE()->GetDevice(), textureImageMemory, nullptr);
}
