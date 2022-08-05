#pragma once

#include "Device.hpp"
#include "BufferDescriptor.hpp"

namespace RightEngine
{
    class Buffer
    {
    public:
        const BufferDescriptor& GetDescriptor() const
        { return descriptor; }

        virtual ~Buffer() = default;

    protected:
        Buffer(const std::shared_ptr<Device>& device, const BufferDescriptor& bufferDescriptor, const void* data) : descriptor(bufferDescriptor)
        {}

        BufferDescriptor descriptor;
    };
}
