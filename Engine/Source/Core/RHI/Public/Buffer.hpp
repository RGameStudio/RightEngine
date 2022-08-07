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

        virtual void* Map() const = 0;
        virtual void UnMap() const = 0;

    protected:
        Buffer(const std::shared_ptr<Device>& device, const BufferDescriptor& bufferDescriptor, const void* data) : descriptor(bufferDescriptor)
        {}

        BufferDescriptor descriptor;
    };
}
