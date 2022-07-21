#pragma once

#include "Device.hpp"
#include <vulkan/vulkan.h>

namespace RightEngine
{
    class VulkanDevice : public Device
    {
    public:
        VulkanDevice(const std::shared_ptr<RenderingContext>& context);
        virtual ~VulkanDevice() override;

    private:
        VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };

        void Init(const std::shared_ptr<RenderingContext>& context);
        void PickPhysicalDevice(VkInstance instance);
    };
}