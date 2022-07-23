#pragma once

#include "Device.hpp"
#include <glm/vec2.hpp>
#include <memory>

namespace RightEngine
{
    enum class Format
    {
        B8G8R8A8_SRGB
    };

    enum class PresentMode
    {
        IMMEDIATE,
        FIFO,
        MAILBOX
    };

    struct SwapchainDescriptor
    {
        Format format;
        PresentMode presentMode;
        glm::ivec2 extent;
    };

    class Swapchain
    {
    public:
        Swapchain(const std::shared_ptr<Device>& device,
                  const std::shared_ptr<Surface>& surface,
                  const SwapchainDescriptor& descriptor) : descriptor(descriptor)
        {}

        virtual ~Swapchain() = default;


        static std::shared_ptr<Swapchain> Create(const std::shared_ptr<Device>& device,
                                                 const std::shared_ptr<Surface>& surface,
                                                 const SwapchainDescriptor& descriptor);

    protected:
        SwapchainDescriptor descriptor;
    };
}
