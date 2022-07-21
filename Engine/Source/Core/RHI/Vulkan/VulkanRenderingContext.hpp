#pragma once

#include "RenderingContext.hpp"
#include <vulkan/vulkan.h>

namespace RightEngine
{
    class VulkanRenderingContext : public RenderingContext
    {
    public:
        VulkanRenderingContext(const std::shared_ptr<Window>& window);

        virtual ~VulkanRenderingContext();

        VkInstance GetInstance() const
        { return instance; }

    private:
        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;

        void Init();
        void SetupDebugMessenger();
    };
}