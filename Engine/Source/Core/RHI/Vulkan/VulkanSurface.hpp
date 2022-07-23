#pragma once

#include "Surface.hpp"
#include "VulkanRenderingContext.hpp"
#include <vulkan/vulkan.h>

namespace RightEngine
{
    class VulkanSurface : public Surface
    {
    public:
        VulkanSurface(const std::shared_ptr<Window>& window,
                const std::shared_ptr<RenderingContext>& context);

        ~VulkanSurface();

        virtual void CreateSwapchain(const std::shared_ptr<Device>& device) override;

        VkSurfaceKHR GetSurface() const
        { return surface; }

    private:
        VkSurfaceKHR surface;
        std::weak_ptr<Window> window;

        void Init(const std::shared_ptr<Window>& window,
                  const std::shared_ptr<VulkanRenderingContext>& context);
    };
}