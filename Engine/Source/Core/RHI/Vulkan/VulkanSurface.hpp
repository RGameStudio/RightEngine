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

        VkSurfaceKHR GetSurface() const
        { return surface; }

    private:
        VkSurfaceKHR surface;

        void Init(const std::shared_ptr<Window>& window,
                  const std::shared_ptr<VulkanRenderingContext>& context);
    };
}