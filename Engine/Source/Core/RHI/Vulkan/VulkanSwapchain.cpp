#include "VulkanSwapchain.hpp"
#include "Assert.hpp"
#include "VulkanConverters.hpp"
#include <glm/glm.hpp>

using namespace RightEngine;

// TODO: Implement vsync on/off here
// VK_PRESENT_MODE_IMMEDIATE_KHR - off
// VK_PRESENT_MODE_MAILBOX_KHR and VK_PRESENT_MODE_FIFO_KHR - on

namespace
{
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
    {
        for (const auto& availableFormat: availableFormats)
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB
                && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities,
                                const SwapchainDescriptor& descriptor)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return capabilities.currentExtent;
        }
        else
        {
            VkExtent2D actualExtent =
                    {
                    static_cast<uint32_t>(descriptor.extent.x),
                    static_cast<uint32_t>(descriptor.extent.y)
            };

            actualExtent.width = glm::clamp(actualExtent.width,
                                            capabilities.minImageExtent.width,
                                            capabilities.maxImageExtent.width);
            actualExtent.height = glm::clamp(actualExtent.height,
                                             capabilities.minImageExtent.height,
                                             capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }
}

VulkanSwapchain::VulkanSwapchain(const std::shared_ptr<Device>& device,
                                 const std::shared_ptr<Surface>& surface,
                                 const SwapchainDescriptor& descriptor) : Swapchain(device, surface, descriptor)
{
    auto vulkanDevice = std::static_pointer_cast<VulkanDevice>(device);
    auto vulkanSurface = std::static_pointer_cast<VulkanSurface>(surface);
    Init(vulkanDevice, vulkanSurface, descriptor);
}

void VulkanSwapchain::Init(const std::shared_ptr<VulkanDevice>& device,
                           const std::shared_ptr<VulkanSurface>& surface,
                           const SwapchainDescriptor& descriptor)
{
    SwapchainSupportDetails details = device->GetSwapchainSupportDetails();

    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(details.formats);
    VkPresentModeKHR presentMode = VulkanConverters::PresentMode(descriptor.presentMode);
    VkExtent2D extent = ChooseSwapExtent(details.capabilities, descriptor);

    uint32_t imageCount = details.capabilities.minImageCount + 1;
    if (details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.maxImageCount)
    {
        imageCount = details.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface->GetSurface();

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = device->FindQueueFamilies();
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = details.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    if (vkCreateSwapchainKHR(device->GetDevice(), &createInfo, nullptr, &swapChain) != VK_SUCCESS)
    {
        R_CORE_ASSERT(false, "Failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(device->GetDevice(), swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device->GetDevice(), swapChain, &imageCount, swapChainImages.data());
}

VulkanSwapchain::~VulkanSwapchain()
{
    auto vulkanDevice = std::static_pointer_cast<VulkanDevice>(Device::Get());
    vkDestroySwapchainKHR(vulkanDevice->GetDevice(), swapChain, nullptr);
}
