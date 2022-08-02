#include "VulkanRendererAPI.hpp"
#include "Assert.hpp"
#include "Application.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanConverters.hpp"

using namespace RightEngine;

namespace
{
    std::vector<VkFramebuffer> framebuffers;
}

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
    SwapchainDescriptor descriptor;
    glm::ivec2 extent;
    glfwGetFramebufferSize(static_cast<GLFWwindow*>(window->GetNativeHandle()), &extent.x, &extent.y);
    descriptor.extent = extent;
    descriptor.format = Format::B8G8R8A8_SRGB;
    descriptor.presentMode = PresentMode::IMMEDIATE;
    swapchain = std::make_shared<VulkanSwapchain>(device, surface, descriptor);

    auto shader = Shader::Create("/Assets/Shaders/simple.vert",
                                 "/Assets/Shaders/simple.frag");
    GraphicsPipelineDescriptor pipelineDescriptor;
    pipelineDescriptor.shader = shader;
    pipelineDescriptor.extent = extent;
    RenderPassDescriptor renderPassDescriptor;
    renderPassDescriptor.format = Format::B8G8R8A8_SRGB;
    pipeline = std::make_shared<VulkanGraphicsPipeline>(pipelineDescriptor, renderPassDescriptor);
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
