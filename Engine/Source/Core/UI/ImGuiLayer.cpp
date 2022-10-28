#include "ImGuiLayer.hpp"

using namespace RightEngine;

ImGuiLayer::ImGuiLayer(): Layer("ImGuiLayer")
{
    CreateImpl();
}

void ImGuiLayer::OnAttach()
{
    impl->OnAttach();
}

void ImGuiLayer::OnDetach()
{
    impl->OnDetach();
}

void ImGuiLayer::OnUpdate(float ts)
{
    impl->OnUpdate(ts);
}

void ImGuiLayer::OnImGuiRender()
{
    impl->OnImGuiRender();
}

void ImGuiLayer::OnEvent(Event& event)
{
    impl->OnEvent(event);
}

void ImGuiLayer::Begin()
{
    impl->Begin();
}

void ImGuiLayer::End()
{
    impl->End();
}
