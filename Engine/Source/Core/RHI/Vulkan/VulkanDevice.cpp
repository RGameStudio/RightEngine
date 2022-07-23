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

        bool IsComplete() const
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
    auto ctx = std::static_pointer_cast<VulkanRenderingContext>(context);
    Init(ctx);
}

void VulkanDevice::Init(const std::shared_ptr<VulkanRenderingContext>& context)
{
    PickPhysicalDevice(context);
    CreateLogicalDevice(context);
    SetupDeviceQueues(context);
}

void VulkanDevice::PickPhysicalDevice(const std::shared_ptr<VulkanRenderingContext>& context)
{
    VkInstance instance = context->GetInstance();
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
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(physicalDevice, &properties);
            R_CORE_INFO(R_VULKAN_LOG("Initialized physical device: {0}"), properties.deviceName);
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE)
    {
        R_CORE_ASSERT(false, "");
    }
}

void VulkanDevice::CreateLogicalDevice(const std::shared_ptr<VulkanRenderingContext>& context)
{
    QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);

    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
    queueCreateInfo.queueCount = 1;
    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = 0;
    const auto validationLayers = context->GetValidationLayers();

    if (validationLayers.size() > 0) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
    {
        R_CORE_ASSERT(false,"Failed to create logical device!");
    }
}

void VulkanDevice::SetupDeviceQueues(const std::shared_ptr<VulkanRenderingContext>& context)
{
    const auto indices = FindQueueFamilies(physicalDevice);
    R_CORE_ASSERT(indices.IsComplete(), "");
    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
}

VulkanDevice::~VulkanDevice()
{
    vkDestroyDevice(device, nullptr);
}
