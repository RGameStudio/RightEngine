#pragma once

#include "Device.hpp"
#include "VulkanRenderingContext.hpp"
#include "VulkanSurface.hpp"
#include <vulkan/vulkan.h>
#include <optional>

namespace RightEngine
{
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool IsComplete() const
        {
            return graphicsFamily.has_value()
                   && presentFamily.has_value();
        }
    };

    struct SwapchainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    class VulkanDevice : public Device
    {
    public:
        VulkanDevice(const std::shared_ptr<RenderingContext>& ctx,
                     const std::shared_ptr<Surface>& surface);
        virtual ~VulkanDevice() override;

        VkPhysicalDevice GetPhysicalDevice() const
        { return physicalDevice; }

        VkDevice GetDevice() const
        { return device; }

        SwapchainSupportDetails GetSwapchainSupportDetails() const;

        QueueFamilyIndices FindQueueFamilies() const;

    private:
        VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };
        VkDevice device{ VK_NULL_HANDLE };
        VkQueue graphicsQueue;
        VkQueue presentQueue;

        void Init(const std::shared_ptr<VulkanRenderingContext>& context);
        void PickPhysicalDevice(const std::shared_ptr<VulkanRenderingContext>& context);
        void CreateLogicalDevice(const std::shared_ptr<VulkanRenderingContext>& context);
        void SetupDeviceQueues(const std::shared_ptr<VulkanRenderingContext>& context);
    };
}