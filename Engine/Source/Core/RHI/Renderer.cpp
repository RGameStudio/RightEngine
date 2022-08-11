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

    ShaderProgramDescriptor shaderProgramDescriptor;
    ShaderDescriptor vertexShader;
    vertexShader.path = "/Assets/Shaders/simple.vert";
    vertexShader.type = ShaderType::VERTEX;
    ShaderDescriptor fragmentShader;
    fragmentShader.path = "/Assets/Shaders/simple.frag";
    fragmentShader.type = ShaderType::FRAGMENT;
    shaderProgramDescriptor.shaders = {vertexShader, fragmentShader};
    VertexBufferLayout layout;
    layout.Push<glm::vec2>();
    layout.Push<glm::vec3>();
    shaderProgramDescriptor.layout = layout;
    const auto shader = Device::Get()->CreateShader(shaderProgramDescriptor);

    const auto window = Application::Get().GetWindow();
    glm::ivec2 extent;
    glfwGetFramebufferSize(static_cast<GLFWwindow*>(window->GetNativeHandle()), &extent.x, &extent.y);
    GraphicsPipelineDescriptor pipelineDescriptor;
    pipelineDescriptor.shader = shader;
    pipelineDescriptor.extent = extent;
    RenderPassDescriptor renderPassDescriptor;
    renderPassDescriptor.format = Format::B8G8R8A8_SRGB;

    pipeline = Device::Get()->CreateGraphicsPipeline(pipelineDescriptor, renderPassDescriptor);
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
    RendererCommand::BeginFrame(commandBuffer, pipeline);
}

void Renderer::EndFrame()
{
    RendererCommand::EndFrame(commandBuffer);
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
                          vertexBuffer->GetDescriptor().size / pipeline->GetPipelineDescriptor().shader->GetShaderProgramDescriptor().layout.GetStride());
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
