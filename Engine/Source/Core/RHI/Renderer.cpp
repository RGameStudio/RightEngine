#include "Renderer.hpp"
#include "RendererCommand.hpp"
#include "Scene.hpp"
#include "Application.hpp"
#include "GraphicsPipeline.hpp"
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
    } else
    {
//        RendererCommand::Draw(vb);
    }
}

void Renderer::BeginFrame(const std::shared_ptr<Camera>& camera)
{
    RendererCommand::BeginFrame(commandBuffer, pipeline);
}

void Renderer::EndFrame()
{
    RendererCommand::EndFrame(commandBuffer, pipeline);
}

void Renderer::Draw(const std::shared_ptr<Buffer>& vertexBuffer, const std::shared_ptr<Buffer>& indexBuffer)
{
    if (indexBuffer)
    {
        RendererCommand::DrawIndexed(commandBuffer, vertexBuffer, indexBuffer, indexBuffer->GetDescriptor().size / sizeof(uint32_t));
        return;
    }
    RendererCommand::Draw(commandBuffer,
                          vertexBuffer,
                          vertexBuffer->GetDescriptor().size /
                          pipeline->GetPipelineDescriptor().shader->GetShaderProgramDescriptor().layout.GetStride());
}

void Renderer::EncodeState(const std::shared_ptr<RendererState>& state)
{
    RendererCommand::EncodeState(commandBuffer, pipeline, state);
}

void Renderer::SetPipeline(const std::shared_ptr<GraphicsPipeline>& aPipeline)
{
    pipeline = aPipeline;
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
