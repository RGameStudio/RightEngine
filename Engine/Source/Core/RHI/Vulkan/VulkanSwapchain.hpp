#pragma once

#include "Swapchain.hpp"

namespace RightEngine
{
    class VulkanSwapchain : public Swapchain
    {
    public:
        VulkanSwapchain(const std::shared_ptr<Device>& device);

        virtual ~VulkanSwapchain();
    };
}
