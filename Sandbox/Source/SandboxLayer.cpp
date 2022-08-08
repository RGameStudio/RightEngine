#include "SandboxLayer.hpp"
#include "RendererCommand.hpp"
#include <glm/gtx/transform.hpp>

using namespace RightEngine;

namespace
{
    struct UBO
    {
        alignas(16) glm::mat4 transform;
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 projection;
    };

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
    std::shared_ptr<Buffer> uboTransform;
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

    BufferDescriptor uboTransformDesc{};
    uboTransformDesc.type = BUFFER_TYPE_UNIFORM;
    uboTransformDesc.size = sizeof(UBO);
    uboTransformDesc.memoryType = static_cast<MemoryType>(MEMORY_TYPE_HOST_COHERENT | MEMORY_TYPE_HOST_VISIBLE);
    uboTransform = Device::Get()->CreateBuffer(uboTransformDesc, nullptr);
}

void SandboxLayer::OnUpdate(float ts)
{
    renderer.BeginFrame(nullptr);
    UBO ubo;
    ubo.transform = glm::rotate(glm::mat4(1.0f), static_cast<float>(glfwGetTime() * glm::radians(90.0f)), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.projection = glm::perspective(glm::radians(45.0f), 1920.0f / 1080, 0.1f, 10.0f);
    ubo.projection[1][1] *= -1;
    auto uboPtr = uboTransform->Map();
    memcpy(uboPtr, &ubo, sizeof(UBO));
    RendererCommand::DrawIndexed(vertexBuffer, indexBuffer);
    uboTransform->UnMap();
    renderer.EndFrame();
}

void SandboxLayer::OnImGuiRender()
{

}
