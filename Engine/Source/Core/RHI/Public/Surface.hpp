#pragma once

#include "Window.hpp"
#include "Device.hpp"
#include "RenderingContext.hpp"

namespace RightEngine
{
    class Surface : public std::enable_shared_from_this<Surface>
    {
    public:
        Surface(const std::shared_ptr<Window>& window,
                const std::shared_ptr<RenderingContext>& context) : context(context)
        {}

        ~Surface() = default;

        virtual void CreateSwapchain(const std::shared_ptr<Device>& device) = 0;

        const std::shared_ptr<Swapchain>& GetSwapchain() const
        { return swapchain; }

        static std::shared_ptr<Surface> Create(const std::shared_ptr<Window>& window,
                      const std::shared_ptr<RenderingContext>& context);

    protected:
        std::weak_ptr<RenderingContext> context;
        std::shared_ptr<Swapchain> swapchain;
    };
}
