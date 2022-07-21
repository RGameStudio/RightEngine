#pragma once

#include "RenderingContext.hpp"

namespace RightEngine
{
    class VulkanRenderingContext : public RenderingContext
    {
    public:
        VulkanRenderingContext(const std::shared_ptr<Window>& window);

        virtual ~VulkanRenderingContext() = default;
    };
}