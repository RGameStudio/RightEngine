#include "VulkanImguiLayerImpl.hpp"
#include "Device.hpp"
#include "Application.hpp"
#include "RendererCommand.hpp"
#include "VulkanDevice.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanUtils.hpp"
#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>
#include <vulkan/vulkan.h>
#include <algorithm>

using namespace RightEngine;

void VulkanImguiLayerImpl::OnAttach()
{
    VkDescriptorPoolSize pool_sizes[] =
            {
                    { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
                    { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
                    { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
                    { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
                    { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
                    { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
                    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
                    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
                    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
                    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
                    { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
            };

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000;
    pool_info.poolSizeCount = std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    const auto vkDevice = VK_DEVICE();

    R_CORE_ASSERT(vkCreateDescriptorPool(vkDevice->GetDevice(), &pool_info, nullptr, &imguiPool) == VK_SUCCESS, "");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

    Application& app = Application::Get();
    GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow()->GetNativeHandle());

    ImGui_ImplGlfw_InitForVulkan(window, true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = std::static_pointer_cast<VulkanRenderingContext>(RendererCommand::GetContext())->GetInstance();
    init_info.PhysicalDevice = vkDevice->GetPhysicalDevice();
    init_info.Device = vkDevice->GetDevice();
    init_info.Queue = vkDevice->GetQueue(QueueType::GRAPHICS);
    init_info.DescriptorPool = imguiPool;
    init_info.MinImageCount = 3;
    init_info.ImageCount = 3;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    VkAttachmentDescription attachment = {};
    attachment.format = VK_FORMAT_B8G8R8A8_SRGB;
    attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment = {};
    color_attachment.attachment = 0;
    color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;  // or VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    info.attachmentCount = 1;
    info.pAttachments = &attachment;
    info.subpassCount = 1;
    info.pSubpasses = &subpass;
    info.dependencyCount = 1;
    info.pDependencies = &dependency;

    if (vkCreateRenderPass(vkDevice->GetDevice(), &info, nullptr, &renderPass) != VK_SUCCESS)
    {
        R_CORE_ASSERT(false, "Could not create Dear ImGui's render pass");
    }

    ImGui_ImplVulkan_Init(&init_info, renderPass);

    const auto cmd = std::static_pointer_cast<VulkanCommandBuffer>(vkDevice->CreateCommandBuffer({ CommandBufferType::GRAPHICS }));
    VulkanUtils::BeginCommandBuffer(cmd, true);
    cmd->Enqueue([](auto buffer)
    {
        ImGui_ImplVulkan_CreateFontsTexture(VK_CMD(buffer)->GetBuffer());
    });
    VulkanUtils::EndCommandBuffer(vkDevice, cmd);

    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void VulkanImguiLayerImpl::OnDetach()
{
    vkDestroyRenderPass(VK_DEVICE()->GetDevice(), renderPass, nullptr);
    vkDestroyDescriptorPool(VK_DEVICE()->GetDevice(), imguiPool, nullptr);
    ImGui_ImplVulkan_Shutdown();
}

void VulkanImguiLayerImpl::OnUpdate(float deltaTime)
{

}

void VulkanImguiLayerImpl::OnImGuiRender()
{

}

void VulkanImguiLayerImpl::OnEvent(Event& event)
{

}

void VulkanImguiLayerImpl::Begin()
{

}

void VulkanImguiLayerImpl::End()
{

}

void ImGuiLayer::CreateImpl()
{
    if (!impl)
    {
        impl = std::make_shared<VulkanImguiLayerImpl>();
    }
}
