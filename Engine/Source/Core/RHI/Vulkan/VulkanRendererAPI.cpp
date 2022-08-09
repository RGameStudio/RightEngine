#include "VulkanRendererAPI.hpp"
#include "Assert.hpp"
#include "Application.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanConverters.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanCommandBuffer.hpp"

using namespace RightEngine;

namespace
{
    const int MAX_FRAMES_IN_FLIGHT = 1;
    uint32_t currentFrame = 0;
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
    glm::ivec2 extent;
    glfwGetFramebufferSize(static_cast<GLFWwindow*>(window->GetNativeHandle()), &extent.x, &extent.y);
    const auto ctx = std::make_shared<VulkanRenderingContext>(window);
    context = ctx;
    surface = std::make_shared<VulkanSurface>(window, context);
    const auto device = Device::Get(context, surface);

    ShaderProgramDescriptor descriptor;
    ShaderDescriptor vertexShader;
    vertexShader.path = "/Assets/Shaders/simple.vert";
    vertexShader.type = ShaderType::VERTEX;
    ShaderDescriptor fragmentShader;
    fragmentShader.path = "/Assets/Shaders/simple.frag";
    fragmentShader.type = ShaderType::FRAGMENT;
    descriptor.shaders = { vertexShader, fragmentShader };
    VertexBufferLayout layout;
    layout.Push<glm::vec2>();
    layout.Push<glm::vec3>();
    descriptor.layout = layout;
    const auto shader = device->CreateShader(descriptor);

    GraphicsPipelineDescriptor pipelineDescriptor;
    pipelineDescriptor.shader = shader;
    pipelineDescriptor.extent = extent;
    RenderPassDescriptor renderPassDescriptor;
    renderPassDescriptor.format = Format::B8G8R8A8_SRGB;
    pipeline = std::make_shared<VulkanGraphicsPipeline>(pipelineDescriptor, renderPassDescriptor);

    CreateSwapchain();
    CreateSyncObjects();
}

void VulkanRendererAPI::CreateSwapchain()
{
    if (swapchain)
    {
        vkDeviceWaitIdle(VK_DEVICE()->GetDevice());
        DestroySwapchain();
    }
    const auto window = Application::Get().GetWindow();
    SwapchainDescriptor descriptor;
    glm::ivec2 extent;
    glfwGetFramebufferSize(static_cast<GLFWwindow*>(window->GetNativeHandle()), &extent.x, &extent.y);
    descriptor.extent = extent;
    descriptor.format = Format::B8G8R8A8_SRGB;
    descriptor.presentMode = PresentMode::IMMEDIATE;
    swapchain = std::make_shared<VulkanSwapchain>(VK_DEVICE(), surface, descriptor);
    CreateFramebuffers();
}

void VulkanRendererAPI::DestroySwapchain()
{
    for (int i = 0; i < swapchainFramebuffers.size(); i++)
    {
        vkDestroyFramebuffer(VK_DEVICE()->GetDevice(), swapchainFramebuffers[i], nullptr);
    }

    swapchain.reset();
}

void VulkanRendererAPI::CreateFramebuffers()
{
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
        framebufferInfo.width = swapchain->GetDescriptor().extent.x;
        framebufferInfo.height = swapchain->GetDescriptor().extent.y;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(VK_DEVICE()->GetDevice(), &framebufferInfo, nullptr, &swapchainFramebuffers[i]) != VK_SUCCESS)
        {
            R_CORE_ASSERT(false,"failed to create framebuffer!");
        }
    }
}

void VulkanRendererAPI::CreateSyncObjects()
{
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(VK_DEVICE()->GetDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(VK_DEVICE()->GetDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(VK_DEVICE()->GetDevice(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {

            R_CORE_ASSERT(false, "failed to create synchronization objects for a frame!");
        }
    }
}

void VulkanRendererAPI::RecordCommandBuffer(const std::shared_ptr<VulkanCommandBuffer>& cmd, uint32_t imageIndex)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer( cmd->GetBuffer(), &beginInfo) != VK_SUCCESS)
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

    VkPipeline vkPipeline = pipeline->GetPipeline();

    cmd->Enqueue([renderPassInfo, vkPipeline](auto buffer)
    {
        vkCmdBeginRenderPass(VK_CMD(buffer)->GetBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(VK_CMD(buffer)->GetBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline);
    });

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapchain->GetDescriptor().extent.x);
    viewport.height = static_cast<float>(swapchain->GetDescriptor().extent.y);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = VulkanConverters::Extent(swapchain->GetDescriptor().extent);

    cmd->Enqueue([viewport, scissor](auto buffer)
    {
         vkCmdSetViewport(VK_CMD(buffer)->GetBuffer(), 0, 1, &viewport);
         vkCmdSetScissor(VK_CMD(buffer)->GetBuffer(), 0, 1, &scissor);
    });
}

// TODO: Add resize callback from GLFW event
// TODO: Add rendering stop when window is minimized
void VulkanRendererAPI::BeginFrame(const std::shared_ptr<CommandBuffer>& cmd)
{
    vkWaitForFences(VK_DEVICE()->GetDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
    VkResult result = vkAcquireNextImageKHR(VK_DEVICE()->GetDevice(),
                          swapchain->GetSwapchain(),
                          UINT64_MAX,
                          imageAvailableSemaphores[currentFrame],
                          VK_NULL_HANDLE,
                          &currentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        CreateSwapchain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        R_CORE_ASSERT(false,"failed to acquire swap chain image!");
    }

    vkResetFences(VK_DEVICE()->GetDevice(), 1, &inFlightFences[currentFrame]);

    const auto vkCommandBuffer = std::static_pointer_cast<VulkanCommandBuffer>(cmd);
    vkResetCommandBuffer(vkCommandBuffer->GetBuffer(), 0);
    RecordCommandBuffer(vkCommandBuffer, currentImageIndex);
}

void VulkanRendererAPI::EndFrame(const std::shared_ptr<CommandBuffer>& cmd)
{
    auto vkCmd = std::static_pointer_cast<VulkanCommandBuffer>(cmd);
    cmd->Enqueue([](auto buffer)
    {
        vkCmdEndRenderPass(VK_CMD(buffer)->GetBuffer());
    });

    cmd->Execute();

    if (vkEndCommandBuffer(vkCmd->GetBuffer()) != VK_SUCCESS)
    {
        R_CORE_ASSERT(false, "failed to record command buffer!");
    }

    // TODO: Move all synchronisation code to command buffer
    VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
    VkCommandBuffer cmdBuffers[] = { vkCmd->GetBuffer() };

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = cmdBuffers;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(VK_DEVICE()->GetQueue(QueueType::GRAPHICS), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
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

    VkResult result = vkQueuePresentKHR(VK_DEVICE()->GetQueue(QueueType::PRESENT), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        CreateSwapchain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        R_CORE_ASSERT(false,"failed to acquire swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
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

void VulkanRendererAPI::Draw(const std::shared_ptr<CommandBuffer>& cmd,
                             const std::shared_ptr<Buffer>& buffer)
{
    const auto vulkanBuffer = std::static_pointer_cast<VulkanBuffer>(buffer);
    VkBuffer vertexBuffers[] = { vulkanBuffer->GetBuffer() };
    VkDeviceSize offsets[] = {0};
    auto pipelinePtr = pipeline;
    cmd->Enqueue([vertexBuffers, offsets, vulkanBuffer, pipeline = pipelinePtr](auto buffer)
    {
        vkCmdBindVertexBuffers(VK_CMD(buffer)->GetBuffer(), 0, 1, vertexBuffers, offsets);
        vkCmdDraw(VK_CMD(buffer)->GetBuffer(),
                  vulkanBuffer->GetDescriptor().size / pipeline->GetPipelineDescriptor().shader->GetShaderProgramDescriptor().layout.GetStride(),
                  1,
                  0,
                  0);
    });
}

void VulkanRendererAPI::Draw(const std::shared_ptr<CommandBuffer>& cmd,
                             const std::shared_ptr<Buffer>& vertexBuffer,
                             const std::shared_ptr<Buffer>& indexBuffer)
{
    const auto vkVertexBuffer = std::static_pointer_cast<VulkanBuffer>(vertexBuffer);
    VkBuffer vertexBuffers[] = { vkVertexBuffer->GetBuffer() };
    VkDeviceSize offsets[] = {0};

    const auto vkIndexBuffer = std::static_pointer_cast<VulkanBuffer>(indexBuffer);

    cmd->Enqueue([vkIndexBuffer, vertexBuffers, offsets](auto buffer)
    {
        vkCmdBindVertexBuffers(VK_CMD(buffer)->GetBuffer(), 0, 1, vertexBuffers, offsets);
        // TODO: Check index size
        vkCmdBindIndexBuffer(VK_CMD(buffer)->GetBuffer(), vkIndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(VK_CMD(buffer)->GetBuffer(),
                         vkIndexBuffer->GetDescriptor().size / sizeof(uint32_t),
                         1,
                         0,
                         0,
                         0);
    });
}

void VulkanRendererAPI::SetClearColor(const glm::vec4& color)
{
    R_CORE_ASSERT(false, "");
}

VulkanRendererAPI::~VulkanRendererAPI()
{
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(VK_DEVICE()->GetDevice(), renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(VK_DEVICE()->GetDevice(), imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(VK_DEVICE()->GetDevice(), inFlightFences[i], nullptr);
    }

    DestroySwapchain();
}
