#include "VulkanBuffer.hpp"
#include "VulkanDevice.hpp"
#include "VulkanUtils.hpp"
#include "VulkanConverters.hpp"
#include <vulkan/vulkan.h>

using namespace RightEngine;

VulkanBuffer::VulkanBuffer(std::shared_ptr<Device> device, const BufferDescriptor& bufferDescriptor, const void* data) : Buffer(device,
                                                                                                                                bufferDescriptor,
                                                                                                                                data)
{
    if (bufferDescriptor.type == BUFFER_TYPE_CONSTANT)
    {
        R_CORE_ASSERT(bufferDescriptor.size < 128, "");
        bufferData = new uint8_t[bufferDescriptor.size];
        return;
    }
    else
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = descriptor.size;
        bufferInfo.usage = VulkanConverters::BufferUsage(bufferDescriptor.type);
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        const auto vulkanDevice = std::static_pointer_cast<VulkanDevice>(device);

        VmaAllocationCreateInfo vmaAllocInfo = {};
        vmaAllocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

        vmaCreateBuffer(vulkanDevice->GetAllocator(), &bufferInfo, &vmaAllocInfo,
                        &buffer,
                        &allocation,
                        nullptr);

//        if (vkCreateBuffer(vulkanDevice->GetDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
//        {
//            R_CORE_ASSERT(false, "failed to create vertex buffer!");
//        }
//
//        VkMemoryRequirements memRequirements;
//        vkGetBufferMemoryRequirements(vulkanDevice->GetDevice(), buffer, &memRequirements);
//        memory = VulkanUtils::AllocateMemory(memRequirements, VulkanConverters::MemoryProperty(bufferDescriptor.memoryType));
//        vkBindBufferMemory(vulkanDevice->GetDevice(), buffer, memory, 0);
    }

    if (data)
    {
        void* bufferPtr = Map();
        memcpy(bufferPtr, data, bufferDescriptor.size);
        UnMap();
    }
}

void* VulkanBuffer::Map() const
{
    if (descriptor.type == BUFFER_TYPE_CONSTANT)
    {
        if (!bufferData)
        {
            bufferData = new uint8_t[descriptor.size];
        }
        return bufferData;
    }
    void* data;
//    vkMapMemory(VK_DEVICE()->GetDevice(), memory, 0, descriptor.size, 0, &data);
    vmaMapMemory(VK_DEVICE()->GetAllocator(), allocation, &data);
    return data;
}

void VulkanBuffer::UnMap() const
{
    if (descriptor.type == BUFFER_TYPE_CONSTANT)
    {
        return;
    }
    vmaUnmapMemory(VK_DEVICE()->GetAllocator(), allocation);
}

VulkanBuffer::~VulkanBuffer()
{
    if (descriptor.type == BUFFER_TYPE_CONSTANT)
    {
        delete[] bufferData;
        return;
    }
    vkDestroyBuffer(VK_DEVICE()->GetDevice(), buffer, nullptr);
//    vkFreeMemory(VK_DEVICE()->GetDevice(), memory, nullptr);
    vmaFreeMemory(VK_DEVICE()->GetAllocator(), allocation);
}
