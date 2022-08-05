#pragma once

#include "Buffer.hpp"

namespace RightEngine
{
    class VulkanBuffer : public Buffer
    {
    public:
        VulkanBuffer(std::shared_ptr<Device> device, const BufferDescriptor& bufferDescriptor, const void* data);

        virtual ~VulkanBuffer() override;
    };
}