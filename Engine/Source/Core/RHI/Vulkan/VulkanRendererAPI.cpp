#include "VulkanRendererAPI.hpp"
#include "Assert.hpp"
#include "Application.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanConverters.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanRendererState.hpp"
#include "VulkanTexture.hpp"
#include "CommandBufferDescriptor.hpp"
#include "VulkanUtils.hpp"

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
    descriptor.format = Format::BGRA8_SRGB;
    descriptor.presentMode = PresentMode::IMMEDIATE;
    swapchain = std::make_shared<VulkanSwapchain>(VK_DEVICE(), surface, descriptor);
}

void VulkanRendererAPI::DestroySwapchain()
{
    for (int i = 0; i < swapchainFramebuffers.size(); i++)
    {
        vkDestroyFramebuffer(VK_DEVICE()->GetDevice(), swapchainFramebuffers[i], nullptr);
    }

    swapchain.reset();
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

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (vkCreateSemaphore(VK_DEVICE()->GetDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(VK_DEVICE()->GetDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(VK_DEVICE()->GetDevice(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
        {

            R_CORE_ASSERT(false, "failed to create synchronization objects for a frame!");
        }
    }
}

// TODO: Add resize callback from GLFW event
// TODO: Add rendering stop when window is minimized
void VulkanRendererAPI::BeginFrame(const std::shared_ptr<CommandBuffer>& cmd,
                                   const std::shared_ptr<GraphicsPipeline>& pipeline)
{
    const auto vkPipeline = std::static_pointer_cast<VulkanGraphicsPipeline>(pipeline);
    vkResetFences(VK_DEVICE()->GetDevice(), 1, &inFlightFences[currentFrame]);

    const auto vkCommandBuffer = std::static_pointer_cast<VulkanCommandBuffer>(cmd);
    vkResetCommandBuffer(vkCommandBuffer->GetBuffer(), 0);
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(vkCommandBuffer->GetBuffer(), &beginInfo) != VK_SUCCESS)
    {
        R_CORE_ASSERT(false, "failed to begin recording command buffer!");
    }

    memset(&renderPassInfo, 0, sizeof(VkRenderPassBeginInfo));
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = vkPipeline->GetRenderPass();
    renderPassInfo.framebuffer = vkPipeline->GetFramebuffer();
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = VulkanConverters::Extent(vkPipeline->GetRenderPassDescriptor().extent);

    static std::vector<VkClearValue> clearValues = {};
    for (const auto& attachment : vkPipeline->GetRenderPassDescriptor().colorAttachments)
    {
        glm::vec4 color = attachment.clearValue.color;
        VkClearValue clearValue;
        clearValue.color = { color.r, color.g, color.b, color.a };
        clearValues.push_back(clearValue);
    }
    VkClearValue clearValue;
    clearValue.depthStencil = { vkPipeline->GetRenderPassDescriptor().depthStencilAttachment.clearValue.depth,
                                vkPipeline->GetRenderPassDescriptor().depthStencilAttachment.clearValue.stencil };
    clearValues.push_back(clearValue);

    renderPassInfo.clearValueCount = clearValues.size();
    renderPassInfo.pClearValues = clearValues.data();

    VkPipeline nativePipeline = vkPipeline->GetPipeline();

    cmd->Enqueue([this, nativePipeline](auto buffer)
                 {
                     vkCmdBeginRenderPass(VK_CMD(buffer)->GetBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
                     vkCmdBindPipeline(VK_CMD(buffer)->GetBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, nativePipeline);
                 });

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(vkPipeline->GetRenderPassDescriptor().extent.x);
    viewport.height = static_cast<float>(vkPipeline->GetRenderPassDescriptor().extent.y);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = VulkanConverters::Extent(vkPipeline->GetRenderPassDescriptor().extent);

    cmd->Enqueue([viewport, scissor](auto buffer)
                 {
                     vkCmdSetViewport(VK_CMD(buffer)->GetBuffer(), 0, 1, &viewport);
                     vkCmdSetScissor(VK_CMD(buffer)->GetBuffer(), 0, 1, &scissor);
                 });
}

void VulkanRendererAPI::EndFrame(const std::shared_ptr<CommandBuffer>& cmd,
                                 const std::shared_ptr<GraphicsPipeline>& pipeline)
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
    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
    VkCommandBuffer cmdBuffers[] = {vkCmd->GetBuffer()};

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = nullptr;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = cmdBuffers;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;

    if (vkQueueSubmit(VK_DEVICE()->GetQueue(QueueType::GRAPHICS), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
    {
        R_CORE_ASSERT(false, "failed to submit draw command buffer!");
    }

    vkWaitForFences(VK_DEVICE()->GetDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    if (!pipeline->GetRenderPassDescriptor().offscreen)
    {
        uint32_t currentImageIndex = 0;
        VkResult result = vkAcquireNextImageKHR(VK_DEVICE()->GetDevice(),
                                                swapchain->GetSwapchain(),
                                                UINT64_MAX,
                                                imageAvailableSemaphores[currentFrame],
                                                VK_NULL_HANDLE,
                                                &currentImageIndex);

        VkImage swapchainImage = swapchain->GetImages()[currentImageIndex];
        const auto texture = std::static_pointer_cast<VulkanTexture>(pipeline->GetRenderPassDescriptor().colorAttachments[0].texture);
        VkImage finalImage = texture->GetImage();

        const auto copyCmdBuffer = std::static_pointer_cast<VulkanCommandBuffer>(Device::Get()->CreateCommandBuffer({ CommandBufferType::GRAPHICS }));
        VulkanUtils::BeginCommandBuffer(copyCmdBuffer, true);
        copyCmdBuffer->Enqueue([&](auto buffer)
        {
            VulkanTexture::ChangeImageLayout(texture->GetImage(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 1, 1);
            VulkanTexture::ChangeImageLayout(swapchainImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, 1);
            memset(&imageCopy, 0, sizeof(VkImageCopy));
            imageCopy.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageCopy.srcSubresource.layerCount = 1;
            imageCopy.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageCopy.dstSubresource.layerCount = 1;
            imageCopy.extent.width = texture->GetSpecification().width;
            imageCopy.extent.height = texture->GetSpecification().height;
            imageCopy.extent.depth = 1;
            vkCmdCopyImage(VK_CMD(buffer)->GetBuffer(), finalImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                           swapchainImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopy);
        });
        VulkanUtils::EndCommandBuffer(VK_DEVICE(), copyCmdBuffer);

//    if (result == VK_ERROR_OUT_OF_DATE_KHR)
//    {
//        CreateSwapchain();
//        return;
//    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
//    {
//        R_CORE_ASSERT(false, "failed to acquire swap chain image!");
//    }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 0;
        presentInfo.pWaitSemaphores = nullptr;
        VkSwapchainKHR swapChains[] = { swapchain->GetSwapchain() };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &currentImageIndex;
        presentInfo.pResults = nullptr; // Optional

        VulkanTexture::ChangeImageLayout(swapchainImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 1, 1);

        result = vkQueuePresentKHR(VK_DEVICE()->GetQueue(QueueType::PRESENT), &presentInfo);

//    if (result == VK_ERROR_OUT_OF_DATE_KHR)
//    {
//        CreateSwapchain();
//        return;
//    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
//    {
//        R_CORE_ASSERT(false, "failed to acquire swap chain image!");
//    }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }
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
                             const std::shared_ptr<Buffer>& buffer,
                             uint32_t vertexCount,
                             uint32_t instanceCount)
{
    const auto vulkanBuffer = std::static_pointer_cast<VulkanBuffer>(buffer);
    VkBuffer vertexBuffers[] = {vulkanBuffer->GetBuffer()};
    VkDeviceSize offsets[] = {0};
    cmd->Enqueue([=](auto buffer)
                 {
                     vkCmdBindVertexBuffers(VK_CMD(buffer)->GetBuffer(), 0, 1, vertexBuffers, offsets);
                     vkCmdDraw(VK_CMD(buffer)->GetBuffer(),
                               vertexCount,
                               instanceCount,
                               0,
                               0);
                 });
}

void VulkanRendererAPI::Draw(const std::shared_ptr<CommandBuffer>& cmd,
                             const std::shared_ptr<Buffer>& vertexBuffer,
                             const std::shared_ptr<Buffer>& indexBuffer,
                             uint32_t indexCount,
                             uint32_t instanceCount)
{
    const auto vkVertexBuffer = std::static_pointer_cast<VulkanBuffer>(vertexBuffer);
    VkBuffer vertexBuffers[] = {vkVertexBuffer->GetBuffer()};
    VkDeviceSize offsets[] = {0};

    const auto vkIndexBuffer = std::static_pointer_cast<VulkanBuffer>(indexBuffer);

    cmd->Enqueue([=](auto buffer)
                 {
                     vkCmdBindVertexBuffers(VK_CMD(buffer)->GetBuffer(), 0, 1, vertexBuffers, offsets);
                     // TODO: Check index size
                     vkCmdBindIndexBuffer(VK_CMD(buffer)->GetBuffer(), vkIndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
                     vkCmdDrawIndexed(VK_CMD(buffer)->GetBuffer(),
                                      indexCount,
                                      instanceCount,
                                      0,
                                      0,
                                      0);
                 });
}

void VulkanRendererAPI::EncodeState(const std::shared_ptr<CommandBuffer>& cmd,
                                    const std::shared_ptr<GraphicsPipeline>& pipeline,
                                    const std::shared_ptr<RendererState>& state)
{
    R_CORE_ASSERT(state, "");
    auto vkState = std::static_pointer_cast<VulkanRendererState>(state);
    auto vkPipeline = std::static_pointer_cast<VulkanGraphicsPipeline>(pipeline);
    cmd->Enqueue([=](auto buffer)
    {
        auto ds = vkState->GetDescriptorSet();
        vkCmdBindDescriptorSets(VK_CMD(buffer)->GetBuffer(),
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                vkPipeline->GetPipelineLayout(),
                                0, 1,
                                &ds,
                                0, nullptr);
    });
}


void VulkanRendererAPI::SetClearColor(const glm::vec4& color)
{
    R_CORE_ASSERT(false, "");
}

VulkanRendererAPI::~VulkanRendererAPI()
{
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(VK_DEVICE()->GetDevice(), renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(VK_DEVICE()->GetDevice(), imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(VK_DEVICE()->GetDevice(), inFlightFences[i], nullptr);
    }

    DestroySwapchain();
}

std::shared_ptr<RendererState> VulkanRendererAPI::CreateRendererState()
{
    return std::make_shared<VulkanRendererState>();
}
