#include "Surface.hpp"
#include "Renderer.hpp"
#include "Vulkan/VulkanSurface.hpp"

using namespace RightEngine;


std::shared_ptr<Surface> Surface::Create(const std::shared_ptr<Window>& window,
                                         const std::shared_ptr<RenderingContext>& context)
{
    switch (Renderer::GetAPI())
    {
        case GPU_API::None:
        case GPU_API::OpenGL:
        R_CORE_ASSERT(false, "");
            break;
        case GPU_API::Vulkan:
            return std::make_shared<VulkanSurface>(window, context);
    }
}
