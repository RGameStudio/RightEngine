#pragma once

#include "Device.hpp"
#include <memory>

namespace RightEngine
{
    class Swapchain
    {
    public:
        Swapchain(const std::shared_ptr<Device>& device)
        {}

        virtual ~Swapchain() = default;

        static std::shared_ptr<Swapchain> Create(const std::shared_ptr<Device>& device);
    };
}
