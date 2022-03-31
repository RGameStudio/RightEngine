#pragma once

#include "Layer.hpp"

namespace RightEngine
{
    class ImGuiLayer : public Layer
    {
    public:
        ImGuiLayer();
        ~ImGuiLayer() override = default;

        virtual void OnAttach() override;
        virtual void OnDetach() override;

        virtual void OnUpdate(float ts) override;
        virtual void OnImGuiRender() override;

        virtual void OnEvent(Event& event) override;

        void Begin();
        void End();
    };
}
