#include "Device.hpp"
#include "Application.hpp"
#include "Vulkan/VulkanDevice.hpp"

using namespace RightEngine;

std::shared_ptr<Device> Device::device{ nullptr };

std::shared_ptr<Device> Device::Get(const std::shared_ptr<RenderingContext>& ctx)
{
    R_CORE_ASSERT(!(device == nullptr && ctx == nullptr), "");

    if (device == nullptr)
    {
        device = std::make_shared<VulkanDevice>(ctx);
    }
    return device;
}
