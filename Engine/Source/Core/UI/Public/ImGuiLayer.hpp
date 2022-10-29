#pragma once

#include "Layer.hpp"
#include "ImguiLayerImpl.hpp"

namespace RightEngine
{
    class ImGuiLayer : public Layer
    {
    public:
        ImGuiLayer(const std::shared_ptr<GraphicsPipeline>& aPipeline);
        ~ImGuiLayer() override = default;

        virtual void OnAttach() override;
        virtual void OnDetach() override;

        virtual void OnUpdate(float ts) override;
        virtual void OnImGuiRender() override;

        virtual void OnEvent(Event& event) override;

        void Begin();
        void End(const std::shared_ptr<CommandBuffer>& cmd);

    private:
        void CreateImpl();

        std::shared_ptr<ImguiLayerImpl> impl;
        std::shared_ptr<GraphicsPipeline> pipeline;
    };
}
