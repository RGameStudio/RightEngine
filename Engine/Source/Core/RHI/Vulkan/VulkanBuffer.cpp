#include "VulkanBuffer.hpp"
#include "VulkanDevice.hpp"
#include "VulkanUtils.hpp"
#include <vulkan/vulkan.h>

using namespace RightEngine;

VulkanBuffer::VulkanBuffer(std::shared_ptr<Device> device, const BufferDescriptor& bufferDescriptor, const void* data) : Buffer(device,
                                                                                                                                bufferDescriptor,
                                                                                                                                data)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = descriptor.size;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    const auto vulkanDevice = std::static_pointer_cast<VulkanDevice>(device);

    if (vkCreateBuffer(vulkanDevice->GetDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
    {
        R_CORE_ASSERT(false, "failed to create vertex buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(vulkanDevice->GetDevice(), buffer, &memRequirements);
    uint32_t flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    memory = VulkanUtils::AllocateMemory(memRequirements, static_cast<VkMemoryPropertyFlagBits>(flags));
    vkBindBufferMemory(vulkanDevice->GetDevice(), buffer, memory, 0);

    void* bufferPtr = Map();
    memcpy(bufferPtr, data, bufferDescriptor.size);
    UnMap();
}

void* VulkanBuffer::Map() const
{
    void* data;
    vkMapMemory(VK_DEVICE()->GetDevice(), memory, 0, descriptor.size, 0, &data);
    return data;
}

void VulkanBuffer::UnMap() const
{
    vkUnmapMemory(VK_DEVICE()->GetDevice(), memory);
}

VulkanBuffer::~VulkanBuffer()
{
    vkDestroyBuffer(VK_DEVICE()->GetDevice(), buffer, nullptr);
    vkFreeMemory(VK_DEVICE()->GetDevice(), memory, nullptr);
}
