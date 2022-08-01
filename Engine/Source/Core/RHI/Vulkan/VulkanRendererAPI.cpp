#include "VulkanRendererAPI.hpp"
#include "Assert.hpp"
#include "Application.hpp"
#include "VulkanRenderingContext.hpp"
#include "VulkanSurface.hpp"

using namespace RightEngine;

void VulkanRendererAPI::Init()
{
    if (isInitialized)
    {
        R_CORE_ASSERT(false, "");
        return;
    }

    if (context)
    {
        R_CORE_ASSERT(false, "");
    }
    const auto window = Application::Get().GetWindow();
    const auto ctx = std::make_shared<VulkanRenderingContext>(window);
    context = ctx;

    surface = std::make_shared<VulkanSurface>(window, context);

    const auto device = Device::Get(context, surface);

    surface->CreateSwapchain(device);
}

void VulkanRendererAPI::Configure(const RendererSettings& settings)
{
    R_CORE_ASSERT(false, "");
}

void VulkanRendererAPI::Clear(uint32_t clearBits)
{
    R_CORE_ASSERT(false, "");
}

void VulkanRendererAPI::SetViewport(const Viewport& viewport)
{
    R_CORE_ASSERT(false, "");
}

Viewport VulkanRendererAPI::GetViewport()
{
    R_CORE_ASSERT(false, "");
    return Viewport();
}

void VulkanRendererAPI::DrawIndexed(const std::shared_ptr<IndexBuffer>& ib)
{
    R_CORE_ASSERT(false, "");
}

void VulkanRendererAPI::Draw(const std::shared_ptr<VertexBuffer>& vb)
{
    R_CORE_ASSERT(false, "");
}

void VulkanRendererAPI::SetClearColor(const glm::vec4& color)
{
    R_CORE_ASSERT(false, "");
}

VulkanRendererAPI::~VulkanRendererAPI()
{

}
