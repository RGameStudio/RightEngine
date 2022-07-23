#pragma once

#include "RenderingContext.hpp"
#include <memory>

namespace RightEngine
{
    class Surface;

    struct DeviceInfo
    {

    };

    class Device : public std::enable_shared_from_this<Device>
    {
    public:
        const DeviceInfo& GetInfo() const
        { return info; }

        virtual ~Device() = default;

        Device(const Device& other) = delete;
        Device& operator=(const Device& other) = delete;
        Device(Device&& other) = delete;
        Device& operator=(Device&& other) = delete;

        static std::shared_ptr<Device> Get(const std::shared_ptr<RenderingContext>& ctx = nullptr,
                                           const std::shared_ptr<Surface>& surface = nullptr);

    protected:
        Device(const std::shared_ptr<RenderingContext>& context, const std::shared_ptr<Surface>& surface)
        {}

        static std::shared_ptr<Device> device;
        DeviceInfo info{};
    };
}
