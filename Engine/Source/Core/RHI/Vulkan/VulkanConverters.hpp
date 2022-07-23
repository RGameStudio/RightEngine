#pragma once

#include "Swapchain.hpp"
#include <vulkan/vulkan.h>

namespace RightEngine
{
    class VulkanConverters
    {
    public:
        static VkPresentModeKHR PresentMode(PresentMode mode)
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
    };
}
