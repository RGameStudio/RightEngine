#pragma once

#include "Window.hpp"
#include "RenderingContext.hpp"

namespace RightEngine
{
    class Surface
    {
    public:
        Surface(const std::shared_ptr<Window>& window,
                const std::shared_ptr<RenderingContext>& context) : context(context)
        {}

        ~Surface() = default;

        static std::shared_ptr<Surface> Create(const std::shared_ptr<Window>& window,
                      const std::shared_ptr<RenderingContext>& context);

    protected:
        std::weak_ptr<RenderingContext> context;
    };
}
