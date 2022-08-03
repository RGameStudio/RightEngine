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

    auto swapchainImageViews = swapchain->GetImageViews();
    swapchainFramebuffers.resize(swapchainImageViews.size());
    for (int i = 0; i < swapchainImageViews.size(); i++)
    {
        VkImageView attachments[] =
                {
                swapchainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = pipeline->GetRenderPass();
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = extent.x;
        framebufferInfo.height = extent.y;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(VK_DEVICE()->GetDevice(), &framebufferInfo, nullptr, &swapchainFramebuffers[i]) != VK_SUCCESS)
        {
            R_CORE_ASSERT(false,"failed to create framebuffer!");
        }
    }

    QueueFamilyIndices queueFamilyIndices = VK_DEVICE()->FindQueueFamilies();

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(VK_DEVICE()->GetDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
    {
        R_CORE_ASSERT(false,"failed to create command pool!");
    }

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(VK_DEVICE()->GetDevice(), &allocInfo, &commandBuffer) != VK_SUCCESS)
    {
        R_CORE_ASSERT(false, "failed to allocate command buffers!");
    }

    CreateSyncObjects();
}

void VulkanRendererAPI::
RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        R_CORE_ASSERT(false, "failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = pipeline->GetRenderPass();
    renderPassInfo.framebuffer = swapchainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = VulkanConverters::Extent(swapchain->GetDescriptor().extent);

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipeline());

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapchain->GetDescriptor().extent.x);
    viewport.height = static_cast<float>(swapchain->GetDescriptor().extent.y);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = VulkanConverters::Extent(swapchain->GetDescriptor().extent);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        R_CORE_ASSERT(false, "failed to record command buffer!");
    }
}

void VulkanRendererAPI::CreateSyncObjects()
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateSemaphore(VK_DEVICE()->GetDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(VK_DEVICE()->GetDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
        vkCreateFence(VK_DEVICE()->GetDevice(), &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS) {
        R_CORE_ASSERT(false, "failed to create semaphores!");
    }
}

void VulkanRendererAPI::BeginFrame()
{
    vkWaitForFences(VK_DEVICE()->GetDevice(), 1, &inFlightFence, VK_TRUE, UINT64_MAX);
    vkAcquireNextImageKHR(VK_DEVICE()->GetDevice(),
                          swapchain->GetSwapchain(),
                          UINT64_MAX,
                          imageAvailableSemaphore,
                          VK_NULL_HANDLE,
                          &currentImageIndex);

    vkResetCommandBuffer(commandBuffer, 0);
    RecordCommandBuffer(commandBuffer, currentImageIndex);
}

void VulkanRendererAPI::EndFrame()
{
    VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(VK_DEVICE()->GetDevice(), 1, &inFlightFence);

    if (vkQueueSubmit(VK_DEVICE()->GetQueue(QueueType::GRAPHICS), 1, &submitInfo, inFlightFence) != VK_SUCCESS)
    {
        R_CORE_ASSERT(false, "failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    VkSwapchainKHR swapChains[] = { swapchain->GetSwapchain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &currentImageIndex;
    presentInfo.pResults = nullptr; // Optional
    vkQueuePresentKHR(VK_DEVICE()->GetQueue(QueueType::PRESENT), &presentInfo);
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
    vkDestroySemaphore(VK_DEVICE()->GetDevice(), imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(VK_DEVICE()->GetDevice(), renderFinishedSemaphore, nullptr);
    vkDestroyFence(VK_DEVICE()->GetDevice(), inFlightFence, nullptr);
    vkDestroyCommandPool(VK_DEVICE()->GetDevice(), commandPool, nullptr);
    for (auto framebuffer : swapchainFramebuffers)
    {
        vkDestroyFramebuffer(VK_DEVICE()->GetDevice(), framebuffer, nullptr);
    }
}
