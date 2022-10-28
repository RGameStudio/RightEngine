#pragma once

#include "ImGuiLayerImpl.hpp"
#include <vulkan/vulkan.h>

namespace RightEngine
{
    class VulkanImguiLayerImpl : public ImguiLayerImpl
    {
    public:
        virtual ~VulkanImguiLayerImpl() override = default;

        virtual void OnAttach() override;
        virtual void OnDetach() override;

        virtual void OnUpdate(float deltaTime) override;
        virtual void OnImGuiRender() override;

        virtual void OnEvent(Event& event) override;

        virtual void Begin() override;
        virtual void End() override;

    private:
        VkRenderPass renderPass;
        VkDescriptorPool imguiPool;
    };
}