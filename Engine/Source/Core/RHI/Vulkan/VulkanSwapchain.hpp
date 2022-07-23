#pragma once

#include "Swapchain.hpp"
#include "VulkanDevice.hpp"

namespace RightEngine
{
    class VulkanSwapchain : public Swapchain
    {
    public:
        VulkanSwapchain(const std::shared_ptr<Device>& device,
                        const std::shared_ptr<Surface>& surface,
                        const SwapchainDescriptor& descriptor);

        virtual ~VulkanSwapchain();

    private:
        VkSwapchainKHR swapChain;
        std::vector<VkImage> swapChainImages;
        std::vector<VkImageView> swapChainImageViews;
        SwapchainSupportDetails details;

        void Init(const std::shared_ptr<VulkanDevice>& device,
                  const std::shared_ptr<VulkanSurface>& surface,
                  const SwapchainDescriptor& descriptor);
    };
}
