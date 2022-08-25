#pragma once

#include "Sampler.hpp"
#include <vulkan/vulkan.h>

namespace RightEngine
{
    class VulkanSampler : public Sampler
    {
    public:
        VulkanSampler(const std::shared_ptr<Device>& device, const SamplerDescriptor& descriptor);

        virtual ~VulkanSampler() override;

    private:
        VkSampler sampler;
    };
}