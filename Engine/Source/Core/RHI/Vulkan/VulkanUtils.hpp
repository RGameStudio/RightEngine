#pragma once

#include <vulkan/vulkan.h>

namespace RightEngine
{
    class VulkanUtils
    {
    public:
        static uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        static VkDeviceMemory AllocateMemory(VkMemoryRequirements memRequirements, VkMemoryPropertyFlagBits flags);
    };
}