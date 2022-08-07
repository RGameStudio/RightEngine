#include "VulkanUtils.hpp"
#include "VulkanDevice.hpp"

using namespace RightEngine;

uint32_t VulkanUtils::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(VK_DEVICE()->GetPhysicalDevice(), &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    R_CORE_ASSERT(false, "failed to find suitable memory type!");
    return -1;
}

VkDeviceMemory VulkanUtils::AllocateMemory(VkMemoryRequirements memRequirements, VkMemoryPropertyFlagBits flags)
{
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, flags);

    VkDeviceMemory memory;

    if (vkAllocateMemory(VK_DEVICE()->GetDevice(), &allocInfo, nullptr, &memory) != VK_SUCCESS)
    {
        R_CORE_ASSERT(false, "failed to allocate vertex buffer memory!");
        return nullptr;
    }

    return memory;
}
