#include "VulkanDevice.hpp"
#include "Assert.hpp"
#include "VulkanRenderingContext.hpp"
#include <optional>
#include <unordered_set>

using namespace RightEngine;

namespace
{
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool IsComplete() const
        {
            return graphicsFamily.has_value()
                   && presentFamily.has_value();
        }
    };

    std::weak_ptr<VulkanSurface> vulkanSurface;

    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily: queueFamilies)
        {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphicsFamily = i;
            }

            if (!vulkanSurface.expired())
            {
                VkBool32 presentSupport = false;
                auto surface = vulkanSurface.lock();
                vkGetPhysicalDeviceSurfaceSupportKHR(device,
                                                     i,
                                                     surface->GetSurface(),
                                                     &presentSupport);

                if (presentSupport)
                {
                    indices.presentFamily = i;
                }
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

VulkanDevice::VulkanDevice(const std::shared_ptr<RenderingContext>& context,
                           const std::shared_ptr<Surface>& surface) : Device(context, surface)
{
    auto ctx = std::static_pointer_cast<VulkanRenderingContext>(context);
    auto surf = std::static_pointer_cast<VulkanSurface>(surface);
    vulkanSurface = surf;
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

    for (const auto& device: devices)
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
    R_CORE_ASSERT(indices.IsComplete(), "");

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::unordered_set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(),
                                                         indices.presentFamily.value() };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = queueCreateInfos.size();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = 0;
    const auto validationLayers = context->GetValidationLayers();

    if (validationLayers.size() > 0)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
    {
        R_CORE_ASSERT(false, "Failed to create logical device!");
    }
}

void VulkanDevice::SetupDeviceQueues(const std::shared_ptr<VulkanRenderingContext>& context)
{
    const auto indices = FindQueueFamilies(physicalDevice);
    R_CORE_ASSERT(indices.IsComplete(), "");
    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

VulkanDevice::~VulkanDevice()
{
    vkDestroyDevice(device, nullptr);
}
