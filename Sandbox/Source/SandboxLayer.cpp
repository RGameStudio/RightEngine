#include "SandboxLayer.hpp"

using namespace RightEngine;

namespace
{
    Renderer renderer;
}

void SandboxLayer::OnAttach()
{
    std::cout << "Hello!" << std::endl;
}

void SandboxLayer::OnUpdate(float ts)
{
    renderer.BeginFrame(nullptr);
    renderer.EndFrame();
}

void SandboxLayer::OnImGuiRender()
{

}
