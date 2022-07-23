#include "VulkanSurface.hpp"
#include "Assert.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"
#include <GLFW/glfw3.h>

using namespace RightEngine;

namespace
{
}

VulkanSurface::VulkanSurface(const std::shared_ptr<Window>& window,
                             const std::shared_ptr<RenderingContext>& context)
        : window(window), Surface(window, context)
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

void VulkanSurface::CreateSwapchain(const std::shared_ptr<Device>& device)
{
    if (window.expired())
    {
        R_CORE_ASSERT(false, "");
        return;
    }

    SwapchainDescriptor descriptor{};
    glfwGetFramebufferSize(static_cast<GLFWwindow*>(window.lock()->GetNativeHandle()),
                           &descriptor.extent.x,
                           &descriptor.extent.y);

    descriptor.format = Format::B8G8R8A8_SRGB;
    descriptor.presentMode = PresentMode::IMMEDIATE;

    swapchain = std::make_shared<VulkanSwapchain>(device, shared_from_this(), descriptor);
}

VulkanSurface::~VulkanSurface()
{
    if (!context.expired())
    {
        auto ctx = std::static_pointer_cast<VulkanRenderingContext>(context.lock());
        vkDestroySurfaceKHR(ctx->GetInstance(), surface, nullptr);
    }
}
