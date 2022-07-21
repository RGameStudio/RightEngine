#pragma once

#include "RenderingContext.hpp"
#include <memory>

namespace RightEngine
{
    struct DeviceInfo
    {

    };

    class Device : public std::enable_shared_from_this<Device>
    {
    public:
        const DeviceInfo& GetInfo() const
        { return info; }

        virtual ~Device() = default;
        static std::shared_ptr<Device> Get(const std::shared_ptr<RenderingContext>& ctx = nullptr);

        Device(const Device& other) = delete;
        Device& operator=(const Device& other) = delete;
        Device(Device&& other) = delete;
        Device& operator=(Device&& other) = delete;

    protected:
        Device(const std::shared_ptr<RenderingContext>& context)
        {}

        static std::shared_ptr<Device> device;
        DeviceInfo info{};
    };
}
