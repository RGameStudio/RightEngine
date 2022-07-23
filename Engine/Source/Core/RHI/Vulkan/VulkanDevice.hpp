#pragma once

#include "Device.hpp"
#include "VulkanRenderingContext.hpp"
#include "VulkanSurface.hpp"
#include <vulkan/vulkan.h>

namespace RightEngine
{
    class VulkanDevice : public Device
    {
    public:
        VulkanDevice(const std::shared_ptr<RenderingContext>& ctx,
                     const std::shared_ptr<Surface>& surface);
        virtual ~VulkanDevice() override;

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