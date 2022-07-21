#include "VulkanDevice.hpp"
#include "Assert.hpp"
#include "VulkanRenderingContext.hpp"
#include <optional>

using namespace RightEngine;

namespace
{
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;

        bool IsComplete()
        {
            return graphicsFamily.has_value();
        }
    };

    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies)
        {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphicsFamily = i;
            }

            if (indices.IsComplete())
            {
                break;
            }

            i++;
        }

        return indices;
    }

    bool IsDeviceSuitable(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices = FindQueueFamilies(device);

        return indices.IsComplete();
    }
}

VulkanDevice::VulkanDevice(const std::shared_ptr<RenderingContext>& context) : Device(context)
{
    Init(context);
    R_CORE_INFO("Successfully created Vulkan device!");
}

void VulkanDevice::Init(const std::shared_ptr<RenderingContext>& context)
{
    VkInstance instance = std::static_pointer_cast<VulkanRenderingContext>(context)->GetInstance();

    PickPhysicalDevice(instance);
}

void VulkanDevice::PickPhysicalDevice(VkInstance instance)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0)
    {
        R_CORE_ASSERT(false, "");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for (const auto& device : devices)
    {
        if (IsDeviceSuitable(device))
        {
            physicalDevice = device;
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE)
    {
        R_CORE_ASSERT(false, "");
    }
}

VulkanDevice::~VulkanDevice()
{

}
