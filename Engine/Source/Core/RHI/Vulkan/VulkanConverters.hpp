#pragma once

#include "VulkanSwapchain.hpp"
#include <glm/vec2.hpp>
#include <vulkan/vulkan.h>

namespace RightEngine
{
    class VulkanConverters
    {
    public:
        inline static VkPresentModeKHR PresentMode(PresentMode mode)
        {
            switch (mode)
            {
                case PresentMode::IMMEDIATE:
                    return VK_PRESENT_MODE_IMMEDIATE_KHR;
                case PresentMode::FIFO:
                    return VK_PRESENT_MODE_FIFO_KHR;
                case PresentMode::MAILBOX:
                    return VK_PRESENT_MODE_MAILBOX_KHR;
            }
        }

        inline static VkExtent2D Extent(const glm::ivec2& extent)
        {
            return { static_cast<uint32_t>(extent.x), static_cast<uint32_t>(extent.y) };
        }

        inline static VkFormat Format(Format format)
        {
            switch (format)
            {
                case Format::B8G8R8A8_SRGB:
                    return VK_FORMAT_B8G8R8A8_SRGB;
            }
        }
    };
}
