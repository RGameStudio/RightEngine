#include "SandboxLayer.hpp"
#include "RendererCommand.hpp"

using namespace RightEngine;

namespace
{
    Renderer renderer;

    const float vertices[] =
            {
                    0.0f, -0.5f, 1.0f, 1.0f, 1.0f,
                    0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
                    -0.5f, 0.5f, 0.0f, 0.0f, 1.0f
            };

    std::shared_ptr<Buffer> vertexBuffer;
}

void SandboxLayer::OnAttach()
{
    std::cout << "Hello!" << std::endl;
    BufferDescriptor bufferDescriptor{};
    bufferDescriptor.type = BufferType::VERTEX;
    bufferDescriptor.size = sizeof(vertices);
    vertexBuffer = Device::Get()->CreateBuffer(bufferDescriptor, vertices);
}

void SandboxLayer::OnUpdate(float ts)
{
    renderer.BeginFrame(nullptr);
    RendererCommand::Draw(vertexBuffer);
    renderer.EndFrame();
}

void SandboxLayer::OnImGuiRender()
{

}
