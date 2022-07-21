#include "RenderingContext.hpp"
#include "Renderer.hpp"
#include "Vulkan/VulkanRenderingContext.hpp"

using namespace RightEngine;

std::shared_ptr<RenderingContext> RenderingContext::Create(const std::shared_ptr<Window>& window)
{
    switch (Renderer::GetAPI())
    {
        case GPU_API::OpenGL:
        R_CORE_ASSERT(false, "");
            break;
        case GPU_API::Vulkan:
            return std::make_shared<VulkanRenderingContext>(window);
    }
}
