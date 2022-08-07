#include "SandboxLayer.hpp"
#include "RendererCommand.hpp"

using namespace RightEngine;

namespace
{
    Renderer renderer;

    const float vertices[] =
            {
                    -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
                    0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
                    0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
                    -0.5f, 0.5f, 0.0f, 0.0f, 1.0f
            };

    const uint32_t indices[] = {
            0, 1, 2,
            2, 3, 0
    };

    std::shared_ptr<Buffer> vertexBuffer;
    std::shared_ptr<Buffer> indexBuffer;
}

void SandboxLayer::OnAttach()
{
    std::cout << "Hello!" << std::endl;
    BufferDescriptor bufferDescriptor{};
    bufferDescriptor.type = BUFFER_TYPE_VERTEX;
    bufferDescriptor.size = sizeof(vertices);
    bufferDescriptor.memoryType = static_cast<MemoryType>(MEMORY_TYPE_HOST_COHERENT | MEMORY_TYPE_HOST_VISIBLE);
    vertexBuffer = Device::Get()->CreateBuffer(bufferDescriptor, vertices);

    BufferDescriptor indexBufferDescriptor{};
    indexBufferDescriptor.type = BUFFER_TYPE_INDEX;
    indexBufferDescriptor.size = sizeof(indices);
    indexBufferDescriptor.memoryType = static_cast<MemoryType>(MEMORY_TYPE_HOST_COHERENT | MEMORY_TYPE_HOST_VISIBLE);
    indexBuffer = Device::Get()->CreateBuffer(indexBufferDescriptor, indices);
}

void SandboxLayer::OnUpdate(float ts)
{
    renderer.BeginFrame(nullptr);
    RendererCommand::DrawIndexed(vertexBuffer, indexBuffer);
    renderer.EndFrame();
}

void SandboxLayer::OnImGuiRender()
{

}
