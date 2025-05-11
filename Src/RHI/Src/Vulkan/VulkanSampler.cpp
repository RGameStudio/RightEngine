#include "VulkanSampler.hpp"
#include "VulkanDevice.hpp"
#include "VulkanHelpers.hpp"

namespace rhi::vulkan
{

    VulkanSampler::VulkanSampler(const SamplerDescriptor& descriptor) : Sampler(descriptor)
    {
        // TODO: Set magFilter, minFilter and mipmapMode through descriptor data

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = helpers::AddressMode(descriptor.m_addressModeU);
        samplerInfo.addressModeV = helpers::AddressMode(descriptor.m_addressModeV);
        samplerInfo.addressModeW = helpers::AddressMode(descriptor.m_addressModeW);
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = VulkanDevice::s_ctx.m_instance->m_parameters.m_maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
#ifdef R_OS_MACOS
        samplerInfo.compareEnable = VK_FALSE;
#else
        samplerInfo.compareEnable = VK_TRUE;
#endif
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = descriptor.m_minLod;
        samplerInfo.maxLod = descriptor.m_maxLod;

        [[maybe_unused]] auto res = vkCreateSampler(VulkanDevice::s_ctx.m_device, &samplerInfo, nullptr, &m_sampler);

        RHI_ASSERT(res == VK_SUCCESS);
    }

    VulkanSampler::~VulkanSampler()
    {
        vkDestroySampler(VulkanDevice::s_ctx.m_device, m_sampler, nullptr);
    }

} // namespace rhi::vulkan
