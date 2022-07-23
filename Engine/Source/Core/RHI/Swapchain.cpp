#include "Swapchain.hpp"
#include "Renderer.hpp"
#include "Vulkan/VulkanSwapchain.hpp"

using namespace RightEngine;

std::shared_ptr<Swapchain> Swapchain::Create(const std::shared_ptr<Device>& device,
                                             const std::shared_ptr<Surface>& surface,
                                             const SwapchainDescriptor& descriptor)
{
    switch (Renderer::GetAPI())
    {
        case GPU_API::None:
        case GPU_API::OpenGL:
        R_CORE_ASSERT(false, "");
            break;
        case GPU_API::Vulkan:
            return std::make_shared<VulkanSwapchain>(device, surface, descriptor);
    }
}
