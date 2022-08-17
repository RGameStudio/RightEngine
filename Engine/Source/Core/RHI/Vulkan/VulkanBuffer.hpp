#pragma once

#include "Buffer.hpp"
#include <vulkan/vulkan.h>

namespace RightEngine
{
    class VulkanBuffer : public Buffer
    {
    public:
        VulkanBuffer(std::shared_ptr<Device> device, const BufferDescriptor& bufferDescriptor, const void* data);

        virtual ~VulkanBuffer() override;

        virtual void* Map() const override;
        virtual void UnMap() const override;

        inline VkBuffer GetBuffer() const
        { return buffer; }

    private:
        VkBuffer buffer;
        VkDeviceMemory memory;
        // Is used only for constant buffer
        mutable uint8_t* bufferData{ nullptr };
    };
}