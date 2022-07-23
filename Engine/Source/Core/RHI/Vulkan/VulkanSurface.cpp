#include "VulkanSurface.hpp"
#include "Assert.hpp"
#include <glfw/glfw3.h>

using namespace RightEngine;

VulkanSurface::VulkanSurface(const std::shared_ptr<Window>& window,
                             const std::shared_ptr<RenderingContext>& context)
        : Surface(window, context)
{
    const auto ctx = std::static_pointer_cast<VulkanRenderingContext>(context);
    Init(window, ctx);
}

void VulkanSurface::Init(const std::shared_ptr<Window>& window,
                         const std::shared_ptr<VulkanRenderingContext>& context)
{
    if (glfwCreateWindowSurface(context->GetInstance(),
                                static_cast<GLFWwindow*>(window->GetNativeHandle()),
                                nullptr,
                                &surface) != VK_SUCCESS)
    {
        R_CORE_ASSERT(false, "Failed to create window surface!");
    }
}

VulkanSurface::~VulkanSurface()
{
    if (!context.expired())
    {
        auto ctx = std::static_pointer_cast<VulkanRenderingContext>(context.lock());
        vkDestroySurfaceKHR(ctx->GetInstance(), surface, nullptr);
    }
}
