#pragma once

#include "ImGuiLayerImpl.hpp"
#include <vulkan/vulkan.h>

namespace RightEngine
{
    class VulkanImguiLayerImpl : public ImguiLayerImpl
    {
    public:
        virtual ~VulkanImguiLayerImpl() override = default;

        virtual void OnAttach(const std::shared_ptr<GraphicsPipeline>& pipeline) override;
        virtual void OnDetach() override;

        virtual void OnUpdate(float deltaTime) override;

        virtual void OnEvent(Event& event) override;

        virtual void Begin() override;
        virtual void End(const std::shared_ptr<CommandBuffer>& cmd) override;

    private:
        VkRenderPass renderPass;
        VkDescriptorPool imguiPool;
    };
}