#include "ImGuiLayer.hpp"

using namespace RightEngine;

ImGuiLayer::ImGuiLayer(const std::shared_ptr<GraphicsPipeline>& aPipeline): pipeline(aPipeline), Layer("ImGuiLayer")
{
    CreateImpl();
}

void ImGuiLayer::OnAttach()
{
    impl->OnAttach(pipeline);
}

void ImGuiLayer::OnDetach()
{
    impl->OnDetach();
}

void ImGuiLayer::OnUpdate(float ts)
{
    impl->OnUpdate(ts);
}

void ImGuiLayer::OnEvent(Event& event)
{
    impl->OnEvent(event);
}

void ImGuiLayer::Begin()
{
    impl->Begin();
}

void ImGuiLayer::End(const std::shared_ptr<CommandBuffer>& cmd)
{
    impl->End(cmd);
}
