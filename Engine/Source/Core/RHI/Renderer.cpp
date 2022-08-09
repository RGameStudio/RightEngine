#include "Renderer.hpp"
#include "RendererCommand.hpp"
#include "Scene.hpp"
#include "OpenGL/OpenGLRendererAPI.hpp"
#include <glm/ext/matrix_clip_space.hpp>

using namespace RightEngine;

Renderer::Renderer()
{
    CommandBufferDescriptor descriptor;
    descriptor.type = CommandBufferType::GRAPHICS;
    commandBuffer = Device::Get()->CreateCommandBuffer(descriptor);
}

void Renderer::SubmitMesh(const std::shared_ptr<Shader>& shader,
                              const MeshComponent& mesh,
                              const glm::mat4& transform)
{
    shader->Bind();
    shader->SetUniformMat4f("u_ViewProjection", sceneData.viewProjectionMatrix);
    shader->SetUniformMat4f("u_Transform", transform);
    const auto& va = mesh.GetVertexArray();
    const auto& ib = va->GetIndexBuffer();
    const auto& vb = va->GetVertexBuffer();
    va->Bind();
    if (ib)
    {
        ib->Bind();
//        RendererCommand::DrawIndexed(ib);
    }
    else
    {
//        RendererCommand::Draw(vb);
    }
}

void Renderer::BeginFrame(const std::shared_ptr<Camera>& camera)
{
    RendererCommand::BeginFrame(commandBuffer);
}

void Renderer::EndFrame()
{
    RendererCommand::EndFrame(commandBuffer);
}

void Renderer::Draw(const std::shared_ptr<Buffer>& vertexBuffer, const std::shared_ptr<Buffer>& indexBuffer)
{
    if (indexBuffer)
    {
        RendererCommand::DrawIndexed(commandBuffer, vertexBuffer, indexBuffer);
        return;
    }
    RendererCommand::Draw(commandBuffer, vertexBuffer);
}

void Renderer::Configure()
{
    RendererCommand::Configure(settings);
}

RendererSettings& Renderer::GetSettings()
{
    return settings;
}

GPU_API Renderer::GetAPI()
{
    return RendererAPI::GetAPI();
}
