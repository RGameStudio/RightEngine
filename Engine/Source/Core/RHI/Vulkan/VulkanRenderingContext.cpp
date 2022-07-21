#include "VulkanRenderingContext.hpp"
#include "Logger.hpp"
#include <vulkan/vulkan.h>

using namespace RightEngine;

VulkanRenderingContext::VulkanRenderingContext(const std::shared_ptr<Window>& window) : RenderingContext(window)
{
    vkEnumerateInstanceExtensionProperties(nullptr, &info.extensionAmount, nullptr);
    R_CORE_INFO("Successfully initialized Vulkan context with {0} extensions!", info.extensionAmount);
}
