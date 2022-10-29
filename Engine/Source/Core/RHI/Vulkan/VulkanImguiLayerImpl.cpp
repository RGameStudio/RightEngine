#include "VulkanImguiLayerImpl.hpp"
#include "Device.hpp"
#include "Application.hpp"
#include "RendererCommand.hpp"
#include "VulkanDevice.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanUtils.hpp"
#include "VulkanGraphicsPipeline.hpp"
#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>
#include <vulkan/vulkan.h>
#include <algorithm>

using namespace RightEngine;

void VulkanImguiLayerImpl::OnAttach(const std::shared_ptr<GraphicsPipeline>& pipeline)
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

    ImGui_ImplVulkan_Init(&init_info, std::static_pointer_cast<VulkanGraphicsPipeline>(pipeline)->GetRenderPass());

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

void VulkanImguiLayerImpl::OnEvent(Event& event)
{

}

void VulkanImguiLayerImpl::Begin()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void VulkanImguiLayerImpl::End(const std::shared_ptr<CommandBuffer>& cmd)
{
    ImGui::Render();
    cmd->Enqueue([](auto buffer)
    {
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(),
                                        VK_CMD(buffer)->GetBuffer());
    });
}

void ImGuiLayer::CreateImpl()
{
    if (!impl)
    {
        impl = std::make_shared<VulkanImguiLayerImpl>();
    }
}
